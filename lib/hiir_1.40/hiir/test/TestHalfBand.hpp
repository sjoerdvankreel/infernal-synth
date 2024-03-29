/*****************************************************************************

        TestHalfBand.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_TestHalfBand_CODEHEADER_INCLUDED)
#define hiir_test_TestHalfBand_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/BlockSplitter.h"
#include "hiir/test/FileOp.h"
#include "hiir/test/ResultCheck.h"
#include "hiir/test/SweepingSine.h"
#include "hiir/test/TestDelay.h"
#include "hiir/PolyphaseIir2Designer.h"

#include <type_traits>

#include <cassert>
#include <cstdio>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
constexpr int	TestHalfBand <TO>::_nbr_chn;
template <class TO>
constexpr int	TestHalfBand <TO>::_nbr_coefs;



template <class TO>
int	TestHalfBand <TO>::perform_test (TO &filter, const double coef_arr [_nbr_coefs], const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at)
{
	assert (coef_arr != nullptr);
	assert (type_0   != nullptr);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);
	assert (stopband_at > 0);

	const char *   datatype_0 =
		  std::is_same <DataType, double>::value ? "double"
		: std::is_same <DataType, float >::value ? "float"
		:                                          "<unknown type>";
	printf (
		"Test: HalfBand, %s, %d chn, %s implementation, %d coefficients.\n",
		datatype_0, _nbr_chn, type_0, _nbr_coefs
	);

	const auto     len = ss.get_len ();
	const auto     src = ResultCheck <DataType>::generate_test_signal (
		ss, len, _nbr_chn
	);

	filter.set_coefs (coef_arr);
	filter.clear_buffers ();

	std::vector <DataType>	dest_lo (len * _nbr_chn, 0);
	std::vector <DataType>	dest_hi (len * _nbr_chn, 0);
	BlockSplitter	bs (64);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Low-pass filtering

	printf ("Low-pass filtering... ");
	fflush (stdout);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long     b_pos = bs.get_pos ();
		const long     b_len = bs.get_len ();
		const int      idx   = b_pos * _nbr_chn;
		filter.process_block (&dest_lo [idx], &src [idx], b_len);
	}
	printf ("Done.\n");

	int            ret_val = 0;
	std::vector <DataType>  dst_chk (len);
	for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
	{
		ret_val = check_band (
			dst_chk, dest_lo, false, chn, ss, type_0, transition_bw, stopband_at,
			"hb_%02d_%s_%dx-%01d_lpf.raw"
		);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// High-pass filtering

	if (ret_val == 0)
	{
		printf ("High-pass filtering... ");
		fflush (stdout);
		for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
		{
			const long     b_pos = bs.get_pos ();
			const long     b_len = bs.get_len ();
			const int      idx   = b_pos * _nbr_chn;
			filter.process_block_hpf (&dest_hi [idx], &src [idx], b_len);
		}
		printf ("Done.\n");

		for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
		{
			ret_val = check_band (
				dst_chk, dest_hi, true, chn, ss, type_0, transition_bw, stopband_at,
				"hb_%02d_%s_%dx-%01d_hpf.raw"
			);
		}
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Splitting

	if (ret_val == 0)
	{
		printf ("Splitting... ");
		fflush (stdout);
		for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
		{
			const long     b_pos = bs.get_pos ();
			const long     b_len = bs.get_len ();
			filter.process_block_split (
				&dest_lo [b_pos * _nbr_chn],
				&dest_hi [b_pos * _nbr_chn],
				&src [b_pos * _nbr_chn],
				b_len
			);
		}
		printf ("Done.\n");
	}

	// Lower band
	for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
	{
		ret_val = check_band (
			dst_chk, dest_lo, false, chn, ss, type_0, transition_bw, stopband_at,
			"hb_%02d_%s_%dx-%01d_lo.raw"
		);
	}

	// Higher band
	for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
	{
		ret_val = check_band (
			dst_chk, dest_hi, true , chn, ss, type_0, transition_bw, stopband_at,
			"hb_%02d_%s_%dx-%01d_hi.raw"
		);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Delay

	if (ret_val == 0)
	{
		printf ("Checking delay... ");
		fflush (stdout);
		ret_val = TestDelay <TO>::test_halfband (filter, coef_arr);
		printf ("Done.\n");
	}

	printf ("\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
int	TestHalfBand <TO>::check_band (std::vector <DataType> &dst_chk, const std::vector <DataType> &dest, bool hi_flag, int chn, const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at, const char *filename_fmt_0)
{
	int            ret_val = 0;

	const long     len_proc = long (dst_chk.size ());
	for (long pos = 0; pos < len_proc; ++pos)
	{
		dst_chk [pos] = dest [pos * _nbr_chn + chn];
	}

	ret_val = ResultCheck <DataType>::check_halfband (
		ss, transition_bw, stopband_at, dst_chk.data (), hi_flag, 1
	);

	char           filename_0 [255+1];
	sprintf (filename_0, filename_fmt_0, _nbr_coefs, type_0, _nbr_chn, chn);
	FileOp <DataType>::save_raw_data (
		filename_0, dst_chk.data (), len_proc, hiir_test_file_resol, 1.f
	);

	return ret_val;
}



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TestHalfBand_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
