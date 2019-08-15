#include "findpeaks.h"

using namespace std;

int cmpdescending (const pair<int, double>& x, const pair<int, double>&y)
{
    return  x.second<y.second;
}
//升序
int cmpascending(const pair<int, double>& x, const pair<int, double>&y)
{
    return  x.second>y.second;
}
//完成MAP到vector 的转化并且完成排序
void sortmapByValue(map<int, double>& tMap, vector<pair<int, double>>& tVector)
{
    for (map<int, double>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
    {
        tVector.push_back(make_pair(curr->first, curr->second));
    }

    sort(tVector.begin(), tVector.end(), cmpascending);
}

//cv::Mat WM(1,end-start,CV_64FC1);
//uchar* ptr= WM.ptr<uchar>(0);
//for(int i= 0;i < end-start;i++)
//{
//    int it= i+start;
//    float v= vmodel->model(it);
//    if(weldLine[i] < v) ptr[i]= 0;
//    else ptr[i]= (int)((weldLine[i]-v)*10+.5000001);
//}
//cout << WM << endl;
//std::map <int, double> mapPeaks_max;
//vector<pair<int, double>> Vector_map_Peaks_max;
//findPeaks(WM, 2, 20, mapPeaks_max, Vector_map_Peaks_max);
//cout << mapPeaks_max.size() << " " << Vector_map_Peaks_max.size() << endl;
////for(auto i:mapPeaks_max) cout << i << end;
//for(auto i:Vector_map_Peaks_max) cout << i.first << " " << i.second << end;
////cv::imwrite("weldPoints"+to_string(subParts[2*i])+"-"+to_string(subParts[2*i+1])+".png",ULV);
//cout << "weldPoints"+to_string(subParts[2*i])+"-"+to_string(subParts[2*i+1])+"..." << endl;

void findPeaks(const Mat Matdata, int minpeakdistance, int minpeakheight, std::map <int, double> &mapPeaks_max, vector<pair<int, double>>&Vector_map_Peaks_max)
{
    int row = Matdata.rows - 1;
    int col = Matdata.cols;
    vector<int> sign;
    vector<double> markdata;
    vector<double> markdata1;
    //minMaxLoc寻找矩阵(一维数组当作向量,用Mat定义) 中最小值和最大值的位置.
    //类型转换注意：data为unchar*
    double* pMatdata = (double*)Matdata.data;
    for (int i = 1; i < col; i++)
    {
        /*相邻值做差：
        *小于0，赋-1
        *大于0，赋1
        *等于0，赋0
        */
        markdata1.push_back(*(pMatdata + i-1));
        int diff = *(pMatdata + i) - *(pMatdata + i-1);
        if (diff > 0)
        {
            sign.push_back(1);
        }
        else if (diff < 0)
        {
            sign.push_back(-1);
        }
        else
        {
            sign.push_back(0);
        }
    }
    //再对sign相邻位做差
    //保存极大值和极小值的位置
    vector<int> indMax;
    vector<int> indMin;
    std::map <int, double> mapPeaks_min;
    for (int j = 1; j < sign.size(); j++)
    {
        int diff = sign[j] - sign[j - 1];
        if (diff < 0)
        {
            indMax.push_back(j);
            //根据峰值最小高度进行筛选
            if ( *(pMatdata +  indMax[indMax.size() - 1]) > minpeakheight){
                mapPeaks_max.insert(pair<int, double>(j, *(pMatdata + indMax[indMax.size() - 1])));
            }

        }
        else if (diff>0)
        {
            indMin.push_back(j);
            mapPeaks_min.insert(pair<int, double>(j, *(pMatdata + indMin[indMin.size() - 1])));

        }
    }

    if (mapPeaks_max.size() >= 2){

        //找到像素值最大的对应的容器指针
        std::map<int, double>::iterator iter_high_max1_pos = std::max_element(std::begin(mapPeaks_max), std::end(mapPeaks_max), cmpdescending);
        int high_max1_pos = iter_high_max1_pos->first;//最高点的位置
        map<int, double>::iterator  iter;
        map<int, double>::iterator  temp;
        //以位置为序
        //map<int, double>::iterator  iter_high_max1_pos;
        map<int, double>::iterator  iter_high_max1_pos_temp1;
        map<int, double>::iterator  iter_high_max1_pos_temp2;
        //知道峰值最高的对应容器指针
        //我们以最高峰值处进行作业搜索所以在此声明两个局部变量保存该指针
        iter_high_max1_pos_temp1= iter_high_max1_pos;
        iter_high_max1_pos_temp2 = iter_high_max1_pos;

        //rate为宽度，以iter_high_max1_pos为起点,根据minpeakdistance的距离倍数搜索
        //minpeakdistance宽度内取一个最大的峰值max_value_temp
        int rate = 1;
        double max_value_temp = 0;
        map<int, double>::iterator max_value_temp_iter;
        //每个minpeakdistance内的第一个指针防止被删除
        bool is_first_max = true;
        //挨着最大峰值的
        if (mapPeaks_max.size() >= 2){
            if (iter_high_max1_pos_temp1 != mapPeaks_max.begin()){

                for (iter = --iter_high_max1_pos_temp1; iter != mapPeaks_max.begin(); iter--){
                    //小于minpeakdistance
                    if (labs((*iter).first - high_max1_pos) < (rate)*minpeakdistance){
                            temp = iter;
                            iter++;
                            mapPeaks_max.erase(temp);
                    }
                    if (rate*minpeakdistance<=labs((*iter).first - high_max1_pos) && labs((*iter).first - high_max1_pos)<=(rate+1)*minpeakdistance){
                        if ((*iter).second > max_value_temp){
                            max_value_temp = (*iter).second;
                            if (is_first_max){
                                max_value_temp_iter = iter;
                                is_first_max = false;
                            }
                            else{
                                //删除原先最大的
                                mapPeaks_max.erase(max_value_temp_iter);
                                temp = iter;
                                iter++;
                                max_value_temp_iter = temp;
                            }
                        }
                        else{
                            is_first_max = true;
                            max_value_temp = 0;
                            temp = iter;
                            iter++;
                            mapPeaks_max.erase(temp);
                        }
                    }
                    //大于（rate + 1）*minpeakdistance
                    else if (labs((*iter).first - high_max1_pos)>(rate + 1)*minpeakdistance){

                            rate++;
                            iter++;
                    }
                }
            }
            is_first_max = true;
            if (iter_high_max1_pos_temp2 != mapPeaks_max.end() && mapPeaks_max.size()>=2){

                for (iter = ++iter_high_max1_pos_temp2; iter != mapPeaks_max.end(); iter++){
                    //TRACE("2Vector:%d \n", debug++);
                    //小于minpeakdistance
                    if (labs((*iter).first - high_max1_pos) < (rate)*minpeakdistance){
                        temp = iter;
                        iter--;
                        mapPeaks_max.erase(temp);
                    }
                    if (rate*minpeakdistance <= labs((*iter).first - high_max1_pos) && labs((*iter).first - high_max1_pos) <= (rate + 1)*minpeakdistance){

                        if ((*iter).second > max_value_temp){
                            max_value_temp = (*iter).second;
                            if (is_first_max){
                                max_value_temp_iter = iter;
                                is_first_max = false;
                            }
                            else{
                                //删除原先最大的
                                mapPeaks_max.erase(max_value_temp_iter);

                                temp = iter;
                                iter--;
                                max_value_temp_iter = temp;
                            }
                        }

                    }
                    //大于（rate + 1）*minpeakdistance
                    else if (labs((*iter).first - high_max1_pos)>(rate + 1)*minpeakdistance){
                        //初始化
                        is_first_max = true;
                        max_value_temp = 0;
                        rate++;
                        iter--;

                    }
                }
                }
            //因为容器指针的begin()指针在上面的循环中未进行判断，所以这里单独处理
            //end()指的是最后一个元素的下一个位置所以不需要淡定进行判断
            map<int, double>::iterator max_value_temp_iter_more;
            if (mapPeaks_max.size() >= 2){
                max_value_temp_iter = mapPeaks_max.begin();
                max_value_temp_iter_more = max_value_temp_iter++;
                if (labs(max_value_temp_iter->first - max_value_temp_iter_more->first) < minpeakdistance){
                    if (max_value_temp_iter->second > max_value_temp_iter_more->second){
                        mapPeaks_max.erase(max_value_temp_iter_more);
                    }
                    else{
                        mapPeaks_max.erase(max_value_temp_iter);
                    }

                }
            }
        }
        Vector_map_Peaks_max.clear();
        //二维向量排序（根据函数选择以key或者value排序）
        sortmapByValue(mapPeaks_max, Vector_map_Peaks_max);
    }

}

void findPeaks(double* num,int count,vector<int>& peaks)
{
   vector<int> sign;
   for(int i = 1;i<count;i++)
   {
       /*相邻值做差：
        *小于0，赋-1
        *大于0，赋1
        *等于0，赋0
        */
       double diff = num[i] - num[i-1];
       if(diff>0)
       {
           sign.push_back(1);
       }
       else if(diff<0)
       {
           sign.push_back(-1);
       }
       else
       {
           sign.push_back(0);
       }
   }
   //再对sign相邻位做差
   //保存极大值和极小值的位置
   vector<int> indMax;
   vector<int> indMin;

   for(int j = 1;j<sign.size();j++)
   {
       int diff = sign[j]-sign[j-1];
       if(diff<0)
       {
           indMax.push_back(j);
       }
       else if(diff>0)
       {
           indMin.push_back(j);
       }
   }
   cout<<"极大值为:"<<endl;
   for(int m = 0;m<indMax.size();m++)
   {
       cout<<num[indMax[m]]<<"  ";
       if(num[indMax[m]] > 4.3) peaks.push_back(indMax[m]);
   }
   cout<<endl;
//   cout<<"极小值为："<<endl;
//   for(int n = 0;n<indMin.size();n++)
//   {
//       cout<<num[indMin[n]]<<"  ";
//   }
}
