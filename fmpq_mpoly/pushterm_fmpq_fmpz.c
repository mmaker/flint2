/*
    Copyright (C) 2018 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpq_mpoly.h"

/*
    emplacebackterm clears c
*/
void _fmpq_mpoly_emplacebackterm_fmpq_fmpz(fmpq_mpoly_t poly,
                      fmpq_t c, fmpz * const * exp, const fmpq_mpoly_ctx_t ctx)
{
    fmpz_mpoly_struct * zpoly = poly->zpoly;

    if (!fmpz_is_one(fmpq_numref(poly->content)))
    {
        _fmpz_vec_scalar_mul_fmpz(zpoly->coeffs, zpoly->coeffs,
                                    zpoly->length, fmpq_numref(poly->content));
        fmpz_one(fmpq_numref(poly->content));
    }

    fmpq_mul_fmpz(c, c, fmpq_denref(poly->content));
    if (!fmpz_is_one(fmpq_denref(c)))
    {
        _fmpz_vec_scalar_mul_fmpz(zpoly->coeffs, zpoly->coeffs,
                                                zpoly->length, fmpq_denref(c));
        fmpz_mul(fmpq_denref(poly->content), fmpq_denref(poly->content),
                                                               fmpq_denref(c));
    }

    _fmpz_mpoly_emplacebackterm_fmpz_fmpz(poly->zpoly, fmpq_numref(c), exp, ctx->zctx);
    fmpz_clear(fmpq_denref(c));
}

void fmpq_mpoly_pushterm_fmpq_fmpz(fmpq_mpoly_t poly,
                const fmpq_t c, fmpz * const * exp, const fmpq_mpoly_ctx_t ctx)
{
    fmpq_t C;
    fmpq_init(C);
    fmpq_set(C, c);
    _fmpq_mpoly_emplacebackterm_fmpq_fmpz(poly, C, exp, ctx);
}

void fmpq_mpoly_pushterm_fmpz_fmpz(fmpq_mpoly_t poly,
                const fmpz_t c, fmpz * const * exp, const fmpq_mpoly_ctx_t ctx)
{
    fmpq_t C;
    fmpz_init_set(fmpq_numref(C), c);
    fmpz_init_set_ui(fmpq_denref(C), UWORD(1));
    _fmpq_mpoly_emplacebackterm_fmpq_fmpz(poly, C, exp, ctx);
}

void fmpq_mpoly_pushterm_ui_fmpz(fmpq_mpoly_t poly,
                       ulong c, fmpz * const * exp, const fmpq_mpoly_ctx_t ctx)
{
    fmpq_t C;
    fmpz_init_set_ui(fmpq_numref(C), c);
    fmpz_init_set_ui(fmpq_denref(C), UWORD(1));
    _fmpq_mpoly_emplacebackterm_fmpq_fmpz(poly, C, exp, ctx);
}

void fmpq_mpoly_pushterm_si_fmpz(fmpq_mpoly_t poly,
                       slong c, fmpz * const * exp, const fmpq_mpoly_ctx_t ctx)
{
    fmpq_t C;
    fmpz_init(fmpq_numref(C));
    fmpz_set_si(fmpq_numref(C), c);
    fmpz_init_set_ui(fmpq_denref(C), UWORD(1));
    _fmpq_mpoly_emplacebackterm_fmpq_fmpz(poly, C, exp, ctx);
}
