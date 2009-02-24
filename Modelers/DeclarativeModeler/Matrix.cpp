//#include "Matrix.h"
//
//template<class T>
//Matrix<T>::Matrix(const Vec3<T> & v)
//{
//	T gamma = v.x();
//	T beta = v.y();
//	T alpha = v.z();
//	
//	T cosgamma = cos(gamma);
//	T singamma = sin(gamma);
//	
//	T cosbeta = cos(beta);
//	T sinbeta = sin(beta);
//	
//	T cosalpha = cos(alpha);
//	T sinalpha = sin(alpha);
//	
//	this(cosgamma, singamma, cosbeta, sinbeta, cosalpha, sinalpha);
//}
//
//template<class T>
//Matrix<T>::Matrix(const T & cosgamma, const T &  singamma, const T &  cosbeta, const T &  sinbeta, const T &  cosalpha, const T &  sinalpha)
//{
//	_data.push_back(std::vector<T>());
//	_data.push_back(std::vector<T>());
//	_data.push_back(std::vector<T>());
//	_data.push_back(std::vector<T>());
//
//	_data[0].push_back(cosalpha * cosbeta);
//	_data[0].push_back(cosalpha * sinbeta * singamma - sinalpha * cosbeta);
//	_data[0].push_back(cosalpha * cosbeta * cosgamma + sinalpha*singamma);
//	_data[0].push_back(0);
//
//	_data[1].push_back(sinalpha * cosbeta);
//	_data[1].push_back(sinalpha * sinbeta*singamma + cosalpha*cosgamma);
//	_data[1].push_back(sinalpha * sinbeta*cosgamma - cosalpha*singamma);
//	_data[1].push_back(0);
//
//	_data[2].push_back(-sinbeta);
//	_data[2].push_back(cosalpha * singamma);
//	_data[2].push_back(cosbeta * cosgamma);
//	_data[2].push_back(0);
//
//	_data[3].push_back(0);
//	_data[3].push_back(0);
//	_data[3].push_back(0);
//	_data[3].push_back(1);
//}
