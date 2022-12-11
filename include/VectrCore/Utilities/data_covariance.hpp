#ifndef VECTRCORE_UTILITIES_DATACOVARIANCE_H
#define VECTRCORE_UTILITIES_DATACOVARIANCE_H

namespace VCTR
{

    /**
     * Simple container class for storing sensor values and their covariances for distribution via topics.
     */
    template <typename VALUETYPE, typename COVARIANCETYPE>
    class DataCovariance
    {
    public:
        /// Values of sensor data.
        VALUETYPE values;

        /// Covariance of sensor values.
        COVARIANCETYPE covariance;

        /**
         * Standard Constructor
        */
        DataCovariance();

        /**
         * covariance values are not set.
         * @param values Values.
        */
        DataCovariance(const VALUETYPE &values);

        /**
         * covariance values are not set.
         * @param values Values.
         * @param covariance Covariance of values.
        */
        DataCovariance(const VALUETYPE &values, const COVARIANCETYPE &covariance);
    };

    template <typename VALUETYPE, typename COVARIANCETYPE>
    DataCovariance<VALUETYPE, COVARIANCETYPE>::DataCovariance() {}

    template <typename VALUETYPE, typename COVARIANCETYPE>
    DataCovariance<VALUETYPE, COVARIANCETYPE>::DataCovariance(const VALUETYPE &values)
    {

        this->values = values;
    }

    template <typename VALUETYPE, typename COVARIANCETYPE>
    DataCovariance<VALUETYPE, COVARIANCETYPE>::DataCovariance(const VALUETYPE &values, const COVARIANCETYPE &covariance)
    {

        this->values = values;
        this->covariance = covariance;
    }

}

#endif