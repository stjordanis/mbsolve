/*
 * mbsolve: Framework for solving the Maxwell-Bloch/-Lioville equations
 *
 * Copyright (c) 2016, Computational Photonics Group, Technical University of
 * Munich.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MBSOLVE_SOURCE_H
#define MBSOLVE_SOURCE_H

#include <string>
#include <vector>
#include <types.hpp>

namespace mbsolve {

/**
 * Base class for all sources in \ref scenario.
 * \ingroup MBSOLVE_LIB
 */
class source
{
public:
    enum type { hard_source, soft_source };

protected:
    std::string m_name;

    /* amplitude */
    real m_ampl;

    /* (carrier) frequency */
    real m_freq;

    /* phase */
    real m_phase;

    /* position */
    real m_position;

    /* internal resistance */
    real m_r_i;

    type m_type;

public:
    /**
     * Constructs source.
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude of the source.
     * \param [in] freq        Frequency of the source.
     * \param [in] phase       Phase of the source.
     */
    source(const std::string& name, real position, type source_type,
           real ampl, real freq, real phase = 0) :
        m_name(name), m_position(position), m_type(source_type),
        m_ampl(ampl), m_freq(freq), m_phase(phase)
    {
    }

    /* TODO: get_value/calc_value : simplify */

    /**
     * Gets source value at certain \p time value.
     *
     * \param [in] t             Time value.
     * \param [in] current_value Reserved for later use.
     */
    real get_value(real t, real current_value = 0.0) const
    {
        /* calculate source value */
        real val = m_ampl * calc_value(t);

        /* if type == thevenin, consider internal resistance */

        /* else if soft source */
        //return current_value + val;

        /* else -> hard source */
        return val;
    }

    /* calculate new value */
    virtual real calc_value(real /* t */) const
    {
        return 0.0;
    }

    /**
     * Gets source position.
     */
    real get_position() const {
        return m_position;
    }

    /**
     * Gets source type.
     */
    type get_type() const {
        return m_type;
    }

    /* TODO: specify dm entry/field etc? */

    /* TODO: add position. how?
	   virtual const Quantity& position
    */
    /* TODO: add source type: hard, soft, thevenin */
    /* TODO: for thevenin: add internal resistance */


};
/*
class sine_source : public source
{
private:

    real calc_value(real t) const
    {
        return sin(2 * PI * m_freq * t + m_phase);
    }

public:
    sine_source(const std::string& name, real ampl, real freq, real phase) :
        source(name, ampl, freq, phase)
    {
    }

    };*/

/**
 * Sech pulse source.
 * \ingroup MBSOLVE_LIB
 */
class sech_pulse : public source
{
private:

    real m_beta;
    real m_phase_sin;

public:
    /**
     * Constructs sech pulse source. The sech pulse is defined as
     * \f[ E(t) = A \sin ( 2 \pi f t - \varphi_1) \mathrm{sech}
     * (\beta t - \varphi_2). \f]
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude \f$ A \f$ of the source.
     * \param [in] freq        Frequency \f$ f \f$ of the source.
     * \param [in] phase       Phase \f$ \varphi_2 \f$ of the source.
     * \param [in] beta        Parameter \f$ \beta \f$.
     * \param [in] phase_sin   Paramter \f$ \varphi_1 \f$.
     */
    sech_pulse(const std::string& name, real position, type source_type,
               real ampl, real freq,
               real phase,
               real beta, real phase_sin = 0.0) :
        source(name, position, source_type, ampl, freq, phase), m_beta(beta),
        m_phase_sin(phase_sin)
    {
    }

    /**
     * Gets source value at certain \p time value.
     *
     * \param [in] t             Time value.
     */
    real calc_value(real t) const
    {
        return 1 / std::cosh(m_beta * t - m_phase) *
            sin(2 * PI * m_freq * t - m_phase_sin);
    }
};

/*
class single_cycle_pulse : public source
{
private:
    real m_beta;

public:
    single_cycle_pulse(const std::string& name, real position,
                       type source_type,
                       real ampl, real freq,
                       real phase,
                       real beta) :
        source(name, position, source_type, ampl, freq, phase), m_beta(beta)
    {
    }

    real calc_value(real t) const
    {
        return 1/std::cosh(m_beta * (t - m_phase)) *
            sin(2 * PI * m_freq * (t - m_phase - 1/(m_freq * 4)));
    }

};
*/

/**
 * Gaussian pulse source.
 * \ingroup MBSOLVE_LIB
 */
class gaussian_pulse : public source
{
private:
    real m_tau;

public:
    /**
     * Constructs Gaussian pulse source. The pulse is defined as
     * \f[ E(t) = A \sin ( 2 \pi f t)
     * \exp\left( - \frac{(t - t_0)^2}{\tau^2} \right) \f]
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude \f$ A \f$ of the source.
     * \param [in] freq        Frequency \f$ f \f$ of the source.
     * \param [in] phase       Phase \f$ t_0 \f$ of the source.
     * \param [in] tau         Parameter \f$ \tau \f$.
     */
    gaussian_pulse(
        const std::string& name,
        real position,
        type source_type,
        real ampl,
        real freq,
        real phase,
        real tau)
      : source(name, position, source_type, ampl, freq, phase), m_tau(tau)
    {
    }

    real calc_value(real t) const
    {
        return exp(-(t - m_phase) * (t - m_phase) / (m_tau * m_tau)) *
            sin(2 * PI * m_freq * t);
    }
};

/* TODO: custom functor source / callback function? */
}

#endif
