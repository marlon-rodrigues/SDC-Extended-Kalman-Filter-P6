#include "kalman_filter.h"

#define PI 3.14159265

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
    * predict the state
  */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
    * update the state by using Kalman Filter equations
  */
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  
  Estimate(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
    * update the state by using Extended Kalman Filter equations
  */
  double px = x_[0];
  double py = x_[1];
  double vx = x_[2];
  double vy = x_[3];
    
  double rho = sqrt(px * px + py * py);
  double phi = atan2(py,  px);
  double rho_dot = (px * vx + py  *vy)/fmax(.0001,rho);
    
  VectorXd hj(3);
  hj << rho, phi, rho_dot;
    
  VectorXd y = z - hj;
    
  //make sure angles are in the desired range (-pi and pi)
  while(y[1] > PI || y[1] < -PI) {
      if(y[1] > PI) {
          y[1] -= 2 * PI;
      } else {
          y[1] += 2 * PI;
      }
  }
    
  Estimate(y);
}

void KalmanFilter::Estimate(const VectorXd y) {
    /**
      * common calculations for both KF and EKF
    */
    
    MatrixXd Ht = H_.transpose();
    MatrixXd S = H_ * P_ * Ht + R_;
    MatrixXd Si = S.inverse();
    MatrixXd PHt = P_ * Ht;
    MatrixXd K = PHt * Si;
    
    //new estimate
    x_ = x_ + (K * y);
    long x_size = x_.size();
    MatrixXd I = MatrixXd::Identity(x_size, x_size);
    P_ = (I - K * H_) * P_;
}
