/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <math.h>
#include <mpir.h>
#include <mpfr.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_vec.h"
#include "arith.h"

#if FLINT64
#define MAX_BELL_TINY 25
#else
#define MAX_BELL_TINY 15
#endif

static const mp_limb_t bell_tiny[] = 
{
    1UL, 1UL, 2UL, 5UL, 15UL, 52UL, 203UL, 877UL, 4140UL, 21147UL, 115975UL,
    678570UL, 4213597UL, 27644437UL, 190899322UL, 1382958545UL,
#if FLINT64
    10480142147UL, 82864869804UL, 682076806159UL, 5832742205057UL,
    51724158235372UL, 474869816156751UL, 4506715738447323UL,
    44152005855084346UL, 445958869294805289UL,
    4638590332229999353UL,
#endif
};


static long _bell_series_cutoff(long n)
{
    double N, log_N, log_pow, log_fac;

    N = n;
    log_N = log(N);
    log_pow = n * log_N;
    log_fac = N*log_N - N;
    while (log_pow - log_fac >= -2)
    {
        N++;
        log_N = log(N);
        log_pow = n * log_N;
        log_fac += log_N;
    }
    return N;
}

void _mpz_bell_bsplit(mpz_t P, mpz_t Q, long a, long b, long n, long bmax)
{
    if (b - a < 20)
    {
        mpz_t u;
        long k;
        mpz_init(u);
        mpz_set_ui(P, 0UL);
        mpz_set_ui(Q, 0UL);
        mpz_set_ui(Q, (b - 1 == bmax) ? 1UL : b);
        for (k = b - 1; k >= a; k--)
        {
            mpz_set_ui(u, k);
            mpz_pow_ui(u, u, n);
            mpz_addmul(P, Q, u);
            if (k != a)
                mpz_mul_ui(Q, Q, k);
        }
        mpz_clear(u);
    }
    else
    {
        long m;
        mpz_t P1, Q2;
        m = (a + b) / 2;
        mpz_init(P1);
        mpz_init(Q2);
        _mpz_bell_bsplit(P1, Q, a, m, n, bmax);
        _mpz_bell_bsplit(P, Q2, m, b, n, bmax);
        mpz_mul(Q, Q, Q2);
        mpz_addmul(P, P1, Q2);
        mpz_clear(P1);
        mpz_clear(Q2);
    }
}

void _fmpz_bell(fmpz_t b, long n)
{
    long N, prec;
    mpz_t P, Q;
    mpfr_t Pf, Qf, E, one;

    N = _bell_series_cutoff(n);

    mpz_init(P);
    mpz_init(Q);

    _mpz_bell_bsplit(P, Q, 1, N + 1, n, N);

    prec = mpz_sizeinbase(P, 2) - mpz_sizeinbase(Q, 2) + 10;

    mpfr_init2(Pf, prec);
    mpfr_init2(Qf, prec);
    mpfr_init2(E, prec);
    mpfr_init2(one, 2);

    mpfr_set_z(Pf, P, GMP_RNDN);
    mpfr_set_z(Qf, Q, GMP_RNDN);
    mpfr_set_ui(one, 1, GMP_RNDN);
    mpfr_exp(E, one, GMP_RNDN);
    mpfr_mul(Qf, Qf, E, GMP_RNDN);
    mpfr_div(Pf, Pf, Qf, GMP_RNDN);
    mpfr_get_z(P, Pf, GMP_RNDN);

    fmpz_set_mpz(b, P);

    mpfr_clear(one);
    mpfr_clear(Pf);
    mpfr_clear(Qf);
    mpfr_clear(E);
    mpz_clear(P);
    mpz_clear(Q);
}

void fmpz_bell(fmpz_t b, ulong n)
{
    if (n <= MAX_BELL_TINY)
        fmpz_set_ui(b, bell_tiny[n]);
    else
        _fmpz_bell(b, n);
}

void fmpz_bell_vec(fmpz * b, long n)
{
    long i, k;
    fmpz * t;

    if (n <= MAX_BELL_TINY)
    {
        for (i = 0; i < n; i++)
            fmpz_set_ui(b + i, bell_tiny[i]);
        return;
    }

    n -= 1;
    t = _fmpz_vec_init(n);

    fmpz_set_ui(t, 1UL);
    fmpz_set_ui(b, 1UL);
    fmpz_set_ui(b + 1, 1UL);

    for (i = 1; i < n; i++)
    {
        fmpz_set(t + i, t);
        for (k = i; k > 0; k--)
            fmpz_add(t + k - 1, t + k - 1, t + k);
        fmpz_set(b + i + 1, t);
    }

    _fmpz_vec_clear(t, n);
}
