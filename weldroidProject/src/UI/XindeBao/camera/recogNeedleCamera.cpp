#include "camera/recogNeedleCamera.h"
//#include <QMessageBox>
#include "GxIAPI.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include "DxImageProc.h"
#include <string>
#include <thread>
#include "base/config.h"

using namespace std;
using namespace std::chrono ;


RecogNeedleCamera::RecogNeedleCamera():
    m_device_handle(nullptr)
  ,m_is_open(false)
  ,m_is_snap(false)
  ,m_color_filter(false)
  ,m_pixel_color(0)
  ,m_image_width(0)
  ,m_image_height(0)
  ,m_payload_size(0)
  ,m_time_out(2000)
  ,m_result_image(NULL)
  ,m_show_image(NULL)
{
    memset(&m_frame_data, 0, sizeof(GX_FRAME_DATA));

    GX_STATUS status = GX_STATUS_SUCCESS;
    status = GXInitLib();
    if (status != GX_STATUS_SUCCESS)
    {
        ShowErrorString(status);
        exit(0);
    }
    connectDevice();
}

RecogNeedleCamera::~RecogNeedleCamera()
{
    GX_STATUS status = GX_STATUS_ERROR;
    if (m_is_open)
    {
        status = CloseDevice();
        GX_VERIFY(status);
    }
    if(m_device_handle != NULL)
    {
        m_device_handle = NULL;
    }
    status = GXCloseLib();
}

void RecogNeedleCamera::connectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!m_is_open)
    {
        bool      ret     = true;
        uint32_t  device_number  = 0;
        GX_STATUS status = GX_STATUS_SUCCESS;
        status = GXUpdateDeviceList(&device_number, UPDATE_TIME_OUT);
        GX_VERIFY(status);
        if (device_number <= 0)
        {
            cout<<"Error, 识别针尖相机 : 当前无设备连接！"<<endl;
            return;
        }
        if (m_device_handle != NULL)
        {
            status = GXCloseDevice(m_device_handle);
            GX_VERIFY(status);
            m_device_handle = NULL;
        }
        status = OpenDevice();
        GX_VERIFY(status);
        m_is_open = true;
        status = InitDevice();
        GX_VERIFY(status);
        status = GetDeviceInitParam();
        GX_VERIFY(status);
        //为图像显示准备资源
        ret = PrepareForShowImg();
        if (!ret)
            return;
        //发送开采命令
        status = GXSendCommand(m_device_handle, GX_COMMAND_ACQUISITION_START);
        GX_VERIFY(status);
        m_is_snap = true;
    }
}

void RecogNeedleCamera::disconnectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS status = GX_STATUS_SUCCESS;

    if(m_is_snap)
    {
        status = GXSendCommand(m_device_handle, GX_COMMAND_ACQUISITION_STOP);
        GX_VERIFY(status);
        m_is_snap = false;
    }

    status = CloseDevice();
    GX_VERIFY(status);

    m_is_open = false;
    m_device_handle = NULL;
}

GX_STATUS RecogNeedleCamera::OpenDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS  status = GX_STATUS_SUCCESS;
    GX_OPEN_PARAM  open_param;

    open_param.accessMode = GX_ACCESS_EXCLUSIVE;
    open_param.openMode   = GX_OPEN_SN;
    open_param.pszContent  = "RH1898008016";
    status = GXOpenDevice(&open_param, &m_device_handle);

    return status;
}

GX_STATUS RecogNeedleCamera::CloseDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS status = GX_STATUS_SUCCESS;
    // 释放资源
    UnPrepareForShowImg();
    // 关闭设备
    status = GXCloseDevice(m_device_handle);
    return status;
}

GX_STATUS RecogNeedleCamera::InitDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS status = GX_STATUS_SUCCESS;
    //设置采集模式连续采集
    status = GXSetEnum(m_device_handle,GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    VERIFY_STATUS_RET(status);
    //设置触发模式为开
    status = GXSetEnum(m_device_handle,GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    VERIFY_STATUS_RET(status);
    //调用SetPixelFormat8bit函数将图像数据格式设置为8Bit
    status = SetPixelFormat8bit();
    VERIFY_STATUS_RET(status);
    //选择触发源为软触发
    //    status = GXSetEnum(m_device_handle,GX_ENUM_TRIGGER_MODE, GX_COMMAND_TRIGGER_SOFTWARE);
    //    GX_VERIFY(status);

    //设置自动白平衡
    //    status=GXSetEnum(m_device_handle,GX_ENUM_BALANCE_WHITE_AUTO,GX_BALANCE_WHITE_AUTO_OFF);
    //    GX_VERIFY(status);

    bool flag = false;
    GXIsImplemented(m_device_handle, GX_ENUM_BALANCE_WHITE_AUTO, &flag);
    if (flag == true)
    {
        //设置白平衡
        status = GXSetEnum(m_device_handle, GX_ENUM_BALANCE_WHITE_AUTO, GX_BALANCE_WHITE_AUTO_OFF);
        GX_VERIFY(status);
    }
    //设置曝光
    Config _sys_config("etc/sys.info");
    int exposure = _sys_config.get<int>("RecogNeedleCamera.Exposure",0);
    setExposure(exposure);

    return status;
}

GX_STATUS RecogNeedleCamera::SetPixelFormat8bit()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS status    = GX_STATUS_SUCCESS;
    int64_t   pixel_size  = 0;
    uint32_t  enum_entry  = 0;
    size_t    buffer_size = 0;
    GX_ENUM_DESCRIPTION  *enum_description = NULL;
    // 获取像素点大小
    status = GXGetEnum(m_device_handle, GX_ENUM_PIXEL_SIZE, &pixel_size);
    if (status != GX_STATUS_SUCCESS)
    {
        return status;
    }
    // 判断为8bit时直接返回,否则设置为8bit
    if (pixel_size == GX_PIXEL_SIZE_BPP8)
    {
        return GX_STATUS_SUCCESS;
    }
    else
    {
        // 获取设备支持的像素格式枚举数
        status = GXGetEnumEntryNums(m_device_handle, GX_ENUM_PIXEL_FORMAT, &enum_entry);
        if (status != GX_STATUS_SUCCESS)
        {
            return status;
        }
        // 为获取设备支持的像素格式枚举值准备资源
        buffer_size      = enum_entry * sizeof(GX_ENUM_DESCRIPTION);
        enum_description = new GX_ENUM_DESCRIPTION[enum_entry];
        // 获取支持的枚举值
        status = GXGetEnumDescription(m_device_handle, GX_ENUM_PIXEL_FORMAT, enum_description, &buffer_size);
        if (status != GX_STATUS_SUCCESS)
        {
            if (enum_description != NULL)
            {
                delete []enum_description;
                enum_description = NULL;
            }
            return status;
        }
        // 遍历设备支持的像素格式,设置像素格式为8bit,
        // 如设备支持的像素格式为Mono10和Mono8则设置其为Mono8
        for (uint32_t i = 0; i < enum_entry; i++)
        {
            if ((enum_description[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
            {
                status = GXSetEnum(m_device_handle, GX_ENUM_PIXEL_FORMAT, enum_description[i].nValue);
                break;
            }
        }
        // 释放资源
        if (enum_description != NULL)
        {
            delete []enum_description;
            enum_description = NULL;
        }
    }
    return status;
}


GX_STATUS RecogNeedleCamera::GetDeviceInitParam()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);

    GX_STATUS status = GX_STATUS_SUCCESS;
    bool      is_implemented = false;
    // 查询当前相机是否支持GX_ENUM_PIXEL_COLOR_FILTER
    status = GXIsImplemented(m_device_handle, GX_ENUM_PIXEL_COLOR_FILTER, &is_implemented);
    VERIFY_STATUS_RET(status);
    m_color_filter = is_implemented;
    //支持彩色图像
    if(is_implemented)
    {
        status = GXGetEnum(m_device_handle, GX_ENUM_PIXEL_COLOR_FILTER, &m_pixel_color);
        VERIFY_STATUS_RET(status);
    }
    //获取图像宽度
    status = GXGetInt(m_device_handle, GX_INT_WIDTH, &m_image_width);
    VERIFY_STATUS_RET(status);
    //获取图像高度
    status = GXGetInt(m_device_handle, GX_INT_HEIGHT, &m_image_height);
    VERIFY_STATUS_RET(status);
    //获取图像大小
    status = GXGetInt(m_device_handle, GX_INT_PAYLOAD_SIZE, &m_payload_size);

    _mat.create(m_image_height,m_image_width,CV_8UC3);
    return status;
}

bool RecogNeedleCamera::PrepareForShowImg()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    bool ret = false;
    m_frame_data.pImgBuf = new char[m_payload_size];
    m_frame_data.nHeight = m_image_height;
    m_frame_data.nWidth = m_image_width;
    m_frame_data.nPixelFormat = m_pixel_color;

    if(m_frame_data.pImgBuf == NULL)
        return ret;
    if(m_color_filter)
    {
        //为彩色图像显示准备资源,分配Buffer
        ret = PrepareForShowColorImg();
    }
    else
    {
        //为黑白图像显示准备资源,分配Buffer
        ret = PrepareForShowMonoImg();
    }

    if(!ret){
        //释放为保存图像分配Buffer,释放为图像显示准备资源
        UnPrepareForShowImg();
    }
    return ret;
}

bool  RecogNeedleCamera::PrepareForShowColorImg()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    //创建QImage对象，用于显示图像
    m_show_image = new QImage(m_image_width, m_image_height, QImage::Format_RGB888);
    if(m_show_image == NULL)
        return false;
    m_result_image = m_show_image->bits();
    return true;
}

/**
\brief 为黑白图像显示准备资源,分配Buffer

\return true:为图像显示准备资源成功  false:准备资源失败
*/
//----------------------------------------------------------------------------------
bool  RecogNeedleCamera::PrepareForShowMonoImg()
{
    //创建QImage对象，用于显示图像
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    m_show_image = new QImage(m_image_width, m_image_height, QImage::Format_RGB888);
    if(m_show_image == NULL)
    {
        return false;
    }

    m_result_image = m_show_image->bits();
    return true;
}


void RecogNeedleCamera::UnPrepareForShowImg()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(m_frame_data.pImgBuf != NULL)
    {
        char *temp_buffer = (char*)m_frame_data.pImgBuf;
        delete[] temp_buffer;
        temp_buffer = NULL;
        m_frame_data.pImgBuf = NULL;
    }
    //释放显示空间
    if (m_show_image != NULL)
    {
        delete m_show_image;
        m_show_image = NULL;
    }
    m_result_image = NULL;
}

cv::Mat RecogNeedleCamera::getImg()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    return const_cast<RecogNeedleCamera*>(this)->takePicture() ;
}

GX_FRAME_DATA RecogNeedleCamera::getFrameDate()
{
    return m_frame_data;
}

cv::Mat RecogNeedleCamera::takePicture()
{
    std::lock_guard<recursive_mutex> lck(_mtx);
    try
    {
        if(m_is_open && m_is_snap && m_device_handle != nullptr)
        {
            GX_STATUS status  = GX_STATUS_ERROR;
            //每次发送触发命令之前清空采集输出队列
            //防止库内部缓存帧，造成本次GXGetImage得到的图像是上次发送触发得到的图
            //            status = GXFlushQueue(m_device_handle);
            //            GX_VERIFY(status);
            //发送软触发命令
            status = GXSendCommand(m_device_handle,GX_COMMAND_TRIGGER_SOFTWARE);
            GX_VERIFY(status);
            //获取图像
            status = GXGetImage(m_device_handle, &m_frame_data, m_time_out);
            GX_VERIFY(status);
            // 获取到的图像是否为完整的图像
            if ((&m_frame_data)->nStatus != 0)
            {
                return _mat;;
                cout<<"图片采集失败"<<endl;
            }
            //若支持彩色,转换为RGB图像后输出
            if (m_color_filter)
            {
                DxRaw8toRGB24((char*)(&m_frame_data)->pImgBuf, m_result_image, m_image_width, m_image_height, RAW2RGB_NEIGHBOUR,
                              DX_PIXEL_COLOR_FILTER(m_pixel_color), false);
            }
            else
            {
                DxRaw8toRGB24((char*)(&m_frame_data)->pImgBuf, m_result_image, m_image_width, m_image_height,RAW2RGB_NEIGHBOUR,
                              DX_PIXEL_COLOR_FILTER(NONE),false);
            }
            _mat.release();
            cv::Mat mat(m_image_height,m_image_width,CV_8UC3);
            memcpy(mat.data,m_result_image,m_image_width*m_image_height*3);
            {
                std::lock_guard<std::recursive_mutex> lck(_mtxMat);
                cv::cvtColor(mat,_mat,CV_RGB2BGR);
            }
            return _mat;
        }else{
            //            BOOST_THROW_EXCEPTION(WZError() << err_camera("连接相机失败")) ;
        }
    }
    catch(...)
    {
        connectDevice();
        disconnectDevice();
    }
    return _mat;
}


void RecogNeedleCamera::ShowErrorString(GX_STATUS error_status)
{
    char*     error_info = NULL;
    size_t    size        = 0;
    GX_STATUS status     = GX_STATUS_ERROR;

    status = GXGetLastError(&error_status, NULL, &size);
    error_info = new char[size];
    if (NULL == error_info)
    {
        return;
    }

    status = GXGetLastError (&error_status, error_info, &size);
    if (status != GX_STATUS_SUCCESS)
    {
        cout<< "Error , GXGetLastError接口调用失败 ! "<<endl;;
    }
    else
    {
        cout <<"Error ,识别针尖相机 : "<<error_info<<endl;;
    }

    if (NULL != error_info)
    {
        delete[] error_info;
        error_info = NULL;
    }
}

void RecogNeedleCamera::setExposure(int exposure)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(m_device_handle == nullptr)
        return;
    GX_STATUS status = GX_STATUS_SUCCESS;
    status = GXSetFloat(m_device_handle, GX_FLOAT_EXPOSURE_TIME,exposure );
    GX_VERIFY(status);
}

void RecogNeedleCamera::setGain( float gain )
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if(m_device_handle == nullptr)
        return;
    GX_STATUS status     = GX_STATUS_ERROR;
    status = GXSetEnum(m_device_handle, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_RED);
    GX_VERIFY(status);

    status = GXSetFloat(m_device_handle, GX_FLOAT_GAIN, gain);
    GX_VERIFY(status);
}

void RecogNeedleCamera::setRoi( float offsetX, float offsetY,int width,int height)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if(m_device_handle == nullptr)
        return;
    GX_STATUS status = GX_STATUS_SUCCESS;
    GX_INT_RANGE stIntRange;

    status = GXGetIntRange(m_device_handle, GX_INT_WIDTH, &stIntRange);
    cout<<"width.range max = "<<stIntRange.nMax<<" , min = "<<stIntRange.nMin<<endl;

    status = GXSetInt(m_device_handle, GX_INT_WIDTH, width);
    GX_VERIFY(status);
    status = GXSetInt(m_device_handle, GX_INT_HEIGHT, height);
    GX_VERIFY(status);
    status = GXSetInt(m_device_handle, GX_INT_OFFSET_X, offsetX);
    GX_VERIFY(status);
    status = GXSetInt(m_device_handle, GX_INT_OFFSET_Y, offsetY);
    GX_VERIFY(status);
}
