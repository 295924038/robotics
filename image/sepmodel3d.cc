/*
 * polymodel3d.cc
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 *  Edit on: March 25 2019
 *      Author: huangweibing
 */

#include "sepmodel3d.h"
#include "nlopt.h"

using namespace walgo;

#define RMS_LIMIT 2

SepModel3D::SepModel3D()
{
    start_t = -1;
    end_t = -1;
    _validTMin=0;
    _validTMax=0;
    next= 0;
}

void SepModel3D::setParams(const std::vector<double>& params)
{
    _modelX.setParams(params);
    _modelY.setParams(params);
    _modelZ.setParams(params);
}

void SepModel3D::init()
{
    _modelX.setData(_x, _t);
    _modelX.build();
    _modelY.setData(_y, _t);
    _modelY.build();
    _modelZ.setData(_z, _t);
    _modelZ.build();
    if (_t.size() > 0)
    {
        _imin = 0;
        _validTMin = _t[_imin];
        _imax = _t.size()-1;
        _validTMax = _t[_imax];
    }
}

void SepModel3D::reBuild(point3d<double> & p0,double t)
{
    if(t > 0 && t< next)
        return;
    vector<double> _x_;
    vector<double> _y_;
    vector<double> _z_;
    deque<double> _t_;
    if(t < 0)
    {
        _x_.push_back(p0._x);
        _y_.push_back(p0._y);
        _z_.push_back(p0._z);
        _t_.push_back(t);
    }
    for(auto ti : _t)
    {
        _x_.push_back(model(ti)._x);
        _y_.push_back(model(ti)._y);
        _z_.push_back(model(ti)._z);
        _t_.push_back(ti);
    }
    if(t > next)
    {
        _x_.push_back(p0._x);
        _y_.push_back(p0._y);
        _z_.push_back(p0._z);
        _t_.push_back(t);
    }
    setData(_x_,_y_,_z_);
    vector<double>().swap(_x_);
    vector<double>().swap(_y_);
    vector<double>().swap(_z_);
    deque<double>().swap(_t);
    _t.clear();
    _t = _t_;
    init();
    deque<double>().swap(_t_);
}

void SepModel3D::build()
{
    lastTvec.clear();
    _t.clear();
    for(int i= 0;i < _lenth;i++)
        _t.push_back(i);
    init();
    initCounts = _lenth;
    start_t = start_t<0?0:start_t;
    end_t = end_t<0?initCounts-1:end_t;
    /// 迭代降低RMS
    while(true)
    {
        vector<double> _x_;
        vector<double> _y_;
        vector<double> _z_;
        deque<double> _t_;
        double rms2 = getRMS();
        rms2 = rms2<0.04?0.04:rms2;
        vector<int> cut_t;
        for(int i= 0;i < _t.size();i++)
        {
            double ti = _t[i];
            if(getMinT(i,ti) < 4*rms2) //3mm以内的
            {
                _x_.push_back(_x[i]);
                _y_.push_back(_y[i]);
                _z_.push_back(_z[i]);
                _t_.push_back(_t[i]);
            }
            else if(_t[i] >= start_t && _t[i] <= end_t)
            {
                cut_t.push_back(_t[i]);
            }
        }
        if(cut_t.size() > 0)
        {
            cutVec.push_back(cut_t);
            rmslimit.push_back(sqrt(rms2));
        }
        if(_x_.size() < 3)
            throw -1;
        setData(_x_,_y_,_z_);
        vector<double>().swap(_x_);
        vector<double>().swap(_y_);
        vector<double>().swap(_z_);
        if(_t.size() == _t_.size() || _t_.size()*1.2 < initCounts)
        {
            _t.clear();
            _t = _t_;
            init();
            deque<double>().swap(_t_);
            break;
        }
        _t.clear();
        _t = _t_;
        init();
        deque<double>().swap(_t_);
    }
    deque<double> temp_t;
    vector<double> temp_x;
    vector<double> temp_y;
    vector<double> temp_z;
    point3d<double> p_pre,p_cur,curdir,direct;
    next= 0;
    cout<<"拟合开始:"<<endl;
    int ts=0,te; ///_t[] index of start & end
    for(int p=0;p<_t.size();p++)
    {
        if(_t[p] <= start_t)
        {
            ts = p;
        }
        if(_t[p] <= end_t)
        {
            te = p;
        }
    }
    cout<<"ts:"<<ts<<"te:"<<te<<endl;
    lastTvec.push_back(_t[ts]);
    p_pre= model(_t[ts]);
    temp_x.push_back(p_pre._x);
    temp_y.push_back(p_pre._y);
    temp_z.push_back(p_pre._z);
    temp_t.push_back(next);
    cout<<p_pre._x<<","<<p_pre._y<<","<<p_pre._z<<","<<next<<endl;
    int m5 = _t.size()-1-ts>5?5:_t.size()-1-ts;
    m5 = m5+ts;
    direct = model(_t[m5])-p_pre;  /// 大方向
    for(int i= 1+ts;i <= te;i++)
    {
        lastTvec.push_back(_t[i]);
        p_cur= model(_t[i]);
        curdir = p_cur-p_pre;
        temp_x.push_back(p_cur._x);
        temp_y.push_back(p_cur._y);
        temp_z.push_back(p_cur._z);
        int sign = 1; // 防止返向
        if(curdir.inner(direct) < 0)
            sign *= -1;
        double cur_next = p_cur.dist(p_pre);
        if(cur_next > 50)
            throw 50;
        next+= sign*cur_next;
        temp_t.push_back(next);
        cout<<p_cur._x<<","<<p_cur._y<<","<<p_cur._z<<","<<next<<endl;
        p_pre= p_cur;
        if(m5 < te)
        {
            m5++;
            direct = model(_t[m5])-p_pre;  /// 大方向
        }
    }
    cout<<"拟合结束"<<endl;
    cout << "长度: " << next;
    if(temp_x.size() < 3)
        throw -1;
    setData(temp_x,temp_y,temp_z);
    vector<double>().swap(temp_x);
    vector<double>().swap(temp_y);
    vector<double>().swap(temp_z);
    deque<double>().swap(_t);
    _t.clear();
    _t= temp_t;
    init();
    deque<double>().swap(temp_t);
}

point3d<double> SepModel3D::model(double T) const
{
    point3d<double> p(_modelX.model(T), _modelY.model(T), _modelZ.model(T));
    return p;
}

double SepModel3D::sepMiss(double t)
{
    point3d<double> pt = {_x[curT],_y[curT],_z[curT]};
    point3d<double> pm = {_modelX.model(t),_modelY.model(t),_modelZ.model(t)};
    point3d<double> ptm = pt-pm;
    return ptm.inner(ptm);
}

double minTdis(unsigned n, const double *x, double *grad, void *data){
    SepModel3D * vss = (SepModel3D *)data;
    // grad[0]= 10*(vss->sepMiss(x[0]+0.05)-vss->sepMiss(x[0]-0.05));
    return vss->sepMiss(x[0]);
}

double SepModel3D::getMinT(double t0, double & ti)
{
    nlopt_opt opt = nlopt_create(NLOPT_LN_SBPLX, 1);
    // nlopt_opt opt = nlopt_create(NLOPT_LD_TNEWTON, 1);
    nlopt_set_min_objective(opt, minTdis, this);
    nlopt_set_stopval(opt, 1e-1);
    nlopt_set_ftol_rel(opt, 1e-1);
    nlopt_set_xtol_rel(opt, 1e-3);
    curT = t0;
    double x[1],mis;
    x[0] = _t[t0];
    nlopt_result result= nlopt_optimize(opt, x, &mis);
    if(!result)
    {
        return mis;
    }
    ti = x[0];
    return mis;
}

double SepModel3D::getRMS() const
{
    int ns = _imax-_imin+1;
    double sumd2 = 0;
    for (int i = _imin; i <= _imax; i++)
    {
        double dx = _x[i] - _modelX.model(_t[i]);
        double dy = _y[i] - _modelY.model(_t[i]);
        double dz = _z[i] - _modelZ.model(_t[i]);
        sumd2 += dx*dx+dy*dy+dz*dz;
    }
    return sumd2/ns;
}

void SepModel3D::setRealSE(int s,int e)
{
    start_t = s;
    end_t = e;
}

void SepModel3D::printTvec(std::vector<int>& tvec)
{
    tvec.clear();
    tvec = lastTvec;
}
