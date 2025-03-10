#pragma once
#include <c_type_traits.h>

CUSTOM_BEGIN

#define E_LONG 2.718281828459045235360287471352662498L
#define LOG2E_LONG 1.442695040888963407359924681001892137L
#define LOG10E_LONG 0.434294481903251827651128918916605082L
#define PI_LONG 3.141592653589793238462643383279502884L
#define INV_PI_LONG 0.318309886183790671537767526745028724L
#define INV_SQRT_PI_LONG 0.564189583547756286948079451560772586L
#define PI_BY_2_LONG 1.570796326794896619231321691639751442L
#define LN2_LONG 0.693147180559945309417232121458176568L
#define LN10_LONG 2.302585092994045684017991454684364208L
#define SQRT2_LONG 1.414213562373095048801688724209698079L
#define SQRT3_LONG 1.732050807568877293527446341505872367L
#define INV_SQRT3_LONG 0.577350269189625764509148780501957456L
#define EGAMMA_LONG 0.577215664901532860606512090082402431L
#define PHI_LONG 1.618033988749894848204586834365638118L


namespace numbers
{
// Templates ============================

    // e
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty e_v = Ty(E_LONG);

    // log_2 e
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty log2e_v = Ty(LOG2E_LONG);

    // log_10 e
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty log10e_v = Ty(LOG10E_LONG);

    // pi
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty pi_v = Ty(PI_LONG);

    // 1/pi
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty inv_pi_v = Ty(INV_PI_LONG);

    // 1/sqrt(pi)
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty inv_sqrtpi_v = Ty(INV_SQRT_PI_LONG);

    // pi/2
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty pi_by_2_v = Ty(PI_BY_2_LONG);

    // log_e 2
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty ln2_v = Ty(LN2_LONG);

    // log_e 10
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty ln10_v = Ty(LN10_LONG);

    // sqrt(2)
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty sqrt2_v = Ty(SQRT2_LONG);

    // sqrt(3)
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty sqrt3_v = Ty(SQRT3_LONG);

    // 1/sqrt(3)
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty inv_sqrt3_v = Ty(INV_SQRT3_LONG);

    // The Euler-Mascheroni constant
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty egamma_v = Ty(EGAMMA_LONG);

    // The golden ratio, (1+sqrt(5))/2
    template<class Ty, enable_if_t<is_floating_point_v<Ty>, bool> = true>
    inline constexpr Ty phi_v = Ty(PHI_LONG);

// Double Specialization ============================

    inline constexpr double e           = e_v<double>;
    inline constexpr double log2e       = log2e_v<double>;
    inline constexpr double log10e      = log10e_v<double>;
    inline constexpr double pi          = pi_v<double>;
    inline constexpr double inv_pi      = inv_pi_v<double>;
    inline constexpr double inv_sqrtpi  = inv_sqrtpi_v<double>;
    inline constexpr double ln2         = ln2_v<double>;
    inline constexpr double ln10        = ln10_v<double>;
    inline constexpr double sqrt2       = sqrt2_v<double>;
    inline constexpr double sqrt3       = sqrt3_v<double>;
    inline constexpr double inv_sqrt3   = inv_sqrt3_v<double>;
    inline constexpr double egamma      = egamma_v<double>;
    inline constexpr double phi         = phi_v<double>;
}   // END namespace numbers

CUSTOM_END