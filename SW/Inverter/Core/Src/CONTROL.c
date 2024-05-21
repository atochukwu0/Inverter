/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CONTROL.c
  * @brief   This file provides code for the control loop.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 David Redondo (@dweggg on GitHub).
  * All rights reserved.
  *
  * This software is licensed under the Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) license.
  * For more information, see: https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
  *
  ******************************************************************************
  */

/* USER CODE END Header */

#include "CONTROL.h"

#include <math.h> // sin/cos, M_PI
#include <PergaMOD.h> // control functions

/**
 * @brief Calculates the current references based on electrical speed, torque reference, voltage reference,
 *        motor parameters, and updates the d-axis and q-axis current references.
 * 
 * @param we         [in] Electrical speed in radians per second.
 * @param torqueRef  [in] Torque reference.
 * @param vsRef      [in] Voltage reference.
 * @param motor      [in] Pointer to the motor parameters structure.
 * @param idRef      [out] Pointer to the d-axis current reference.
 * @param iqRef      [out] Pointer to the q-axis current reference.
 */
void calc_current_reference(float we, float torqueRef, float vsRef, MotorParameters * motor, volatile float * idRef, volatile float * iqRef){

    static float gammaRef = M_PI_2;
    static float isRef = 0.0F;

    static int8_t signTorqueRef = 1;

    float isRefCTC;
    float gammaRefMTPA;

	if (torqueRef >= 0){
		signTorqueRef = 1;
	} else {
		signTorqueRef = -1;
	}

	torqueRef = fabs(torqueRef);


    // CTC
    if (gammaRef == M_PI_2 || torqueRef == 0.0F || motor->Ld == motor->Lq) {
        isRefCTC = 2.0F * torqueRef * motor->constants.invThreePpLambda;
    } else {
        float sinGammaRef = sinf(gammaRef);
        float sin2GammaRef = sinf(2.0F * gammaRef);
        float temp = motor->constants.fourTimesOneMinusXi * torqueRef * motor->constants.invTorqueBase;
        isRefCTC = motor->constants.isc * (sqrtf(sinGammaRef * sinGammaRef + sin2GammaRef * temp) - sinGammaRef) / (sin2GammaRef * motor->constants.oneMinusXi);
    }

    // isRef = min(isRefCTC .... iMax)
    isRef = (isRefCTC < motor->iMax) ? isRefCTC : motor->iMax;

    // MTPA
    if (isRef == 0.0F || motor->Ld == motor->Lq) {
        gammaRefMTPA = M_PI_2;
    } else {
        gammaRefMTPA = M_PI_2 + asinf((motor->lambda - sqrtf(motor->constants.eightTimesOneMinusXiSquared * isRef * isRef + motor->lambda * motor->lambda)) / (motor->constants.fourTimesOneMinusXi * isRef));
    }

    gammaRef = gammaRefMTPA*signTorqueRef;

    // Polar to Cartesian
    *idRef = isRef * cosf(gammaRef);
    *iqRef = isRef * sinf(gammaRef);
}



/**
 * @brief Calculates the id-iq loops.
 *
 * @param inv Pointer to the inverter structure.
 */
void calc_current_loop(volatile InverterStruct *inv){

    inv->idLoop.pi_out_max = inv->vsMax;
    inv->idLoop.pi_out_min = -(inv->vsMax);

    // inv->idLoop.pi_ffw[0] = - (inv->feedback.iqMeas * inv->motor->Lq * inv->encoder.we); // Feedforward
    inv->idLoop.pi_consig = inv->reference.idRef;  	   // Setpoint
    inv->idLoop.pi_fdb = inv->feedback.idMeas;         // Feedback

    pi_calc(&(inv->idLoop));                            // Calculate id PI controller output



    inv->iqLoop.pi_out_max = inv->vsMax;
    inv->iqLoop.pi_out_min = -(inv->vsMax);


    // inv->iqLoop.pi_ffw[0] =  (inv->feedback.idMeas * inv->motor->Ld + inv->motor->lambda) * inv->encoder.we; // Feedforward
    inv->iqLoop.pi_consig = inv->reference.iqRef;  	   // Setpoint
    inv->iqLoop.pi_fdb = inv->feedback.iqMeas;         // Feedback

    pi_calc(&(inv->iqLoop));   // Calculate iq PI controller output


}

/**
 * @brief Saturates PI output to not surpass DC voltage.
 *
 * @param inv Pointer to the inverter structure.
 */
void saturate_voltage(volatile InverterStruct *inv){

	inv->vd = inv->idLoop.pi_out;
	inv->vq = inv->iqLoop.pi_out;

	float vsRef = sqrtf(inv->vd*inv->vd + inv->vq*inv->vq);
	float invVsRef = 1.0F/vsRef;
	if (vsRef > inv->vsMax){
		inv->vq = inv->vq*inv->vsMax*invVsRef;
		inv->vd = inv->vd*inv->vsMax*invVsRef;
	}

}
/**
 * @brief function.
 *
 * This function calculates the inverse Park transform and the duty cycles using SVPWM
 *
 * @param[in] vd Voltage in the d-axis.
 * @param[in] vq Voltage in the q-axis.
 * @param[in] vDC DC voltage.
 * @param[in] sinTheta_e Electrical angle sine (-1..1)
 * @param[in] cosTheta_e Electrical angle cosine (-1..1)
 * @param[out] duties Pointer to the duties structure.
 */
void calc_duties(float vd, float vq, float vDC, float sinTheta_e, float cosTheta_e, volatile Duties *duties) {

  // inverse Park transform
  float alpha = (vd/vDC)*cosTheta_e - (vq/vDC)*sinTheta_e;              // Alpha(D) = d*cos(Fi) - q*sin(Fi)
  float beta = (vd/vDC)*sinTheta_e + (vq/vDC)*cosTheta_e;              // Beta(Q) = d*sin(Fi) + q*cos(Fi)


  svpwm_struct svpwm;

  // Assign values to SVPWM structure, works with alpha/beta (not a/b/c)
  // alpha and beta are meant to be in the range +-(1/sqrt3)
  svpwm.alpha = alpha;
  svpwm.beta = beta;
  svpwm_calc(&svpwm);

  // Assign SVPWM duties (0 to 1, high side)
  duties->Da = svpwm.Da;
  duties->Db = svpwm.Db;
  duties->Dc = svpwm.Dc;

}
