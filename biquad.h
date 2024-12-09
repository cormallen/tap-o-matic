//
//  Biquad.cpp
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//

#include <math.h>

#define M_PI 3.14159265358979323846

enum {
    bq_type_lowpass = 0,
    bq_type_lowpass_1pole,
    bq_type_lowpass_1p1z,
    bq_type_highpass,
    bq_type_highpass_1pole,
    bq_type_highpass_1p1z,
    bq_type_bandpass,
    bq_type_notch,
    bq_type_peak,
    bq_type_lowshelf,
    bq_type_highshelf,
    bq_type_allpass
};

class Biquad {
public:
    Biquad() {
        type = bq_type_lowpass;
        a0 = 1.0;
        a1 = a2 = b1 = b2 = 0.0;
        Fc = 0.50;
        Q = 0.707;
        peakGain = 0.0;
        z1 = z2 = 0.0;
    }

    Biquad(int type, float Fc, float Q, float peakGainDB) {
        setBiquad(type, Fc, Q, peakGainDB);
        z1 = z2 = 0.0;
    }

    ~Biquad() {
    }

    void setType(int type) {
        this->type = type;
        calcBiquad();
    }

    void setQ(float Q) {
        this->Q = Q;
        calcBiquad();
    }

    void setFc(float Fc) {
        this->Fc = Fc;
        calcBiquad();
    }

    void setPeakGain(float peakGainDB) {
        this->peakGain = peakGainDB;
        calcBiquad();
    }

    void setBiquad(int type, float Fc, float Q, float peakGainDB) {
        this->type = type;
        this->Q = Q;
        this->Fc = Fc;
        setPeakGain(peakGainDB);
    }

    void calcBiquad(void) {
        float norm;
        float V = pow(10, fabs(peakGain) / 20.0);
        float K = tan(M_PI * Fc);
        switch (this->type) {
            case bq_type_lowpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K * K * norm;
                a1 = 2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;

		    case bq_type_lowpass_1pole:
			    norm = 1 / (1 / K + 1);
			    a0 = a1 = norm;
			    b1 = (1 - 1 / K) * norm;
			    a2 = b2 = 0;
			    break;

            case bq_type_lowpass_1p1z:
                norm = 1 / (1 / K + 1);
                a0 = a1 = norm;
                b1 = (1 - 1 / K) * norm;
                a2 = b2 = 0;
                break;

            case bq_type_highpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = 1 * norm;
                a1 = -2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;

		    case bq_type_highpass_1pole:
			    norm = 1 / (K + 1);
			    a0 = norm;
			    a1 = -norm;
			    b1 = (K - 1) * norm;
			    a2 = b2 = 0;
			    break;

		    case bq_type_highpass_1p1z:
			    norm = 1 / (K + 1);
			    a0 = norm;
			    a1 = -norm;
			    b1 = (K - 1) * norm;
			    a2 = b2 = 0;
			    break;

            case bq_type_bandpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K / Q * norm;
                a1 = 0;
                a2 = -a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;

            case bq_type_notch:
                norm = 1 / (1 + K / Q + K * K);
                a0 = (1 + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = a0;
                b1 = a1;
                b2 = (1 - K / Q + K * K) * norm;
                break;

            case bq_type_peak:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + 1/Q * K + K * K);
                    a0 = (1 + V/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - V/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - 1/Q * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + V/Q * K + K * K);
                    a0 = (1 + 1/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - 1/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - V/Q * K + K * K) * norm;
                }
                break;
            case bq_type_lowshelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
                    a1 = 2 * (V * K * K - 1) * norm;
                    a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (V * K * K - 1) * norm;
                    b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                }
                break;
            case bq_type_highshelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (V + sqrt(2*V) * K + K * K) * norm;
                    a1 = 2 * (K * K - V) * norm;
                    a2 = (V - sqrt(2*V) * K + K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (V + sqrt(2*V) * K + K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (K * K - V) * norm;
                    b2 = (V - sqrt(2*V) * K + K * K) * norm;
                }
                break;
            case bq_type_allpass:
                norm = 1 / (1 + K * (1.0 / Q) + (K*K));
                a0 = (1 - K * (1.0 / Q) + (K*K)) * norm;
                a1 = 2 * ((K*K) - 1) * norm;
                a2 = 1;
                b1 = a1;
                b2 = a0;
                break;
        }

        return;
    }

    inline float process(float in) {
        float out = in * a0 + z1;
        z1 = in * a1 + z2 - b1 * out;
        z2 = in * a2 - b2 * out;
        return out;
    }

protected:

    int type;
    float a0, a1, a2, b1, b2;
    float Fc, Q, peakGain;
    float z1, z2;
};

