#line 1 "E:\\1.Data\\1.AutoCad\\1_Nam 2025\\May thu bao nylon\\Code\\may_canbaobi_ver3.0\\libraries\\SimpleKalmanFilter-master\\src\\SimpleKalmanFilter.h"
/*
 * SimpleKalmanFilter - a Kalman Filter implementation for single variable models.
 * Created by Denys Sene, January, 1, 2017.
 * Released under MIT License - see LICENSE file for details.
 */

#ifndef SimpleKalmanFilter_h
#define SimpleKalmanFilter_h

class SimpleKalmanFilter
{

public:
  SimpleKalmanFilter(float mea_e, float est_e, float q);
  float updateEstimate(float mea);
  void setMeasurementError(float mea_e);
  void setEstimateError(float est_e);
  void setProcessNoise(float q);
  float getKalmanGain();
  float getEstimateError();
  float deleteEstimate(); // Ham duoc them vao.
  float update(float _u); // Ham duoc them vao.


private:
  float _err_measure;
  float _err_estimate;
  float _q;
  float _current_estimate;
  float _last_estimate;
  float _kalman_gain;
  //float _u;

};

#endif
