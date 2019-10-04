import pytest
import numpy as np
from datetime import datetime

import dlisio

def load_curves(fpath):
    with dlisio.load(fpath) as (f, *_):
        frame = f.object('FRAME', 'FRAME-REPRCODE', 10, 0)
        curves = frame.curves()
        return curves

def test_fshort():
    fpath = 'data/fdata/reprcodes/01-fshort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-1))

def test_fsingl():
    fpath = 'data/fdata/reprcodes/02-fsingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(5.5))

def test_fsing1():
    fpath = 'data/fdata/reprcodes/03-fsing1.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((-2, 2)))

def test_fsing2():
    fpath = 'data/fdata/reprcodes/04-fsing2.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((117, -13.25, 32444)))

def test_isingl():
    fpath = 'data/fdata/reprcodes/05-isingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-12))

def test_vsingl():
    fpath = 'data/fdata/reprcodes/06-vsingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(0.125))

def test_fdoubl():
    fpath = 'data/fdata/reprcodes/07-fdoubl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(900000000000000.5))

def test_fdoub1():
    fpath = 'data/fdata/reprcodes/08-fdoub1.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((-13.5, -27670)))

def test_fdoub2():
    fpath = 'data/fdata/reprcodes/09-fdoub2.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((6728332223, -45.75, -0.0625)))

def test_csingl():
    fpath = 'data/fdata/reprcodes/10-csingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(complex(93, -14)))

def test_cdoubl():
    fpath = 'data/fdata/reprcodes/11-cdoubl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(complex(125533556, -4.75)))

def test_sshort():
    fpath = 'data/fdata/reprcodes/12-sshort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(89))

def test_snorm():
    fpath = 'data/fdata/reprcodes/13-snorm.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-153))

def test_slong():
    fpath = 'data/fdata/reprcodes/14-slong.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(2147483647))

def test_ushort():
    fpath = 'data/fdata/reprcodes/15-ushort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(6))

def test_unorm():
    fpath = 'data/fdata/reprcodes/16-unorm.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(32921))

def test_ulong():
    fpath = 'data/fdata/reprcodes/17-ulong.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(1))

@pytest.mark.xfail(strict=True)
def test_uvari():
    fpath = 'data/fdata/reprcodes/18-uvari.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(257))

@pytest.mark.xfail(strict=True)
def test_ident():
    fpath = 'data/fdata/reprcodes/19-ident.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("VALUE"))

@pytest.mark.xfail(strict=True)
def test_ascii():
    fpath = 'data/fdata/reprcodes/20-ascii.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("Thou shalt not kill"))

@pytest.mark.xfail(strict=True)
def test_dtime():
    fpath = 'data/fdata/reprcodes/21-dtime.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(datetime(1971, 3, 21, 18, 4, 14, 386)))

@pytest.mark.xfail(strict=True)
def test_origin():
    fpath = 'data/fdata/reprcodes/22-origin.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(16777217))

@pytest.mark.xfail(strict=True)
def test_obname():
    fpath = 'data/fdata/reprcodes/23-obname.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((18, 5, "OBNAME_I")))

@pytest.mark.xfail(strict=True)
def test_objref():
    fpath = 'data/fdata/reprcodes/24-objref.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(("OBJREF_I", (25, 3, "OBJREF_OBNAME"))))

@pytest.mark.xfail(strict=True)
def test_attref():
    fpath = 'data/fdata/reprcodes/25-attref.dlis'
    curves = load_curves(fpath)
    ex_attref = ("FIRST_INDENT", (3, 2, "ATTREF_OBNAME"), "SECOND_INDENT")
    np.testing.assert_array_equal(
        curves[0][0], np.array(ex_attref))

def test_status():
    fpath = 'data/fdata/reprcodes/26-status.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(True))

@pytest.mark.xfail(strict=True)
def test_units():
    fpath = 'data/fdata/reprcodes/27-units.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("unit"))


@pytest.mark.xfail(strict=True)
def test_fshort_x2():
    fpath = 'data/fdata/reprcodes-x2/01-fshort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-1))
    np.testing.assert_array_equal(
        curves[1][0], np.array(153))

@pytest.mark.xfail(strict=True)
def test_fsingl_x2():
    fpath = 'data/fdata/reprcodes-x2/02-fsingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(5.5))
    np.testing.assert_array_equal(
        curves[1][0], np.array(-13.75))

@pytest.mark.xfail(strict=True)
def test_fsing1_x2():
    fpath = 'data/fdata/reprcodes-x2/03-fsing1.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((-2, 2)))
    np.testing.assert_array_equal(
        curves[1][0], np.array((-2, 3.5)))

@pytest.mark.xfail(strict=True)
def test_fsing2_x2():
    fpath = 'data/fdata/reprcodes-x2/04-fsing2.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((117, -13.25, 32444)))
    np.testing.assert_array_equal(
        curves[1][0], np.array((3524454, 10, 20)))

@pytest.mark.xfail(strict=True)
def test_isingl_x2():
    fpath = 'data/fdata/reprcodes-x2/05-isingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-12))
    np.testing.assert_array_equal(
        curves[1][0], np.array(65536.5))

@pytest.mark.xfail(strict=True)
def test_vsingl_x2():
    fpath = 'data/fdata/reprcodes-x2/06-vsingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(0.125))
    np.testing.assert_array_equal(
        curves[1][0], np.array(-26.5))

@pytest.mark.xfail(strict=True)
def test_fdoubl_x2():
    fpath = 'data/fdata/reprcodes-x2/07-fdoubl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(900000000000000.5))
    np.testing.assert_array_equal(
        curves[1][0], np.array(-153))

@pytest.mark.xfail(strict=True)
def test_fdoub1_x2():
    fpath = 'data/fdata/reprcodes-x2/08-fdoub1.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((-13.5, -27670)))
    np.testing.assert_array_equal(
        curves[1][0], np.array((5673345, 14)))

@pytest.mark.xfail(strict=True)
def test_fdoub2_x2():
    fpath = 'data/fdata/reprcodes-x2/09-fdoub2.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((6728332223, -45.75, -0.0625)))
    np.testing.assert_array_equal(
        curves[1][0], np.array((95637722454, 20, 5)))

@pytest.mark.xfail(strict=True)
def test_csingl_x2():
    fpath = 'data/fdata/reprcodes-x2/10-csingl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(complex(93, -14)))
    np.testing.assert_array_equal(
        curves[1][0], np.array(complex(67, -37)))

@pytest.mark.xfail(strict=True)
def test_cdoubl_x2():
    fpath = 'data/fdata/reprcodes-x2/11-cdoubl.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(complex(125533556, -4.75)))
    np.testing.assert_array_equal(
        curves[1][0], np.array(complex(67, -37)))

@pytest.mark.xfail(strict=True)
def test_sshort_x2():
    fpath = 'data/fdata/reprcodes-x2/12-sshort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(89))
    np.testing.assert_array_equal(
        curves[1][0], np.array(-89))

@pytest.mark.xfail(strict=True)
def test_snorm_x2():
    fpath = 'data/fdata/reprcodes-x2/13-snorm.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-153))
    np.testing.assert_array_equal(
        curves[1][0], np.array(153))

@pytest.mark.xfail(strict=True)
def test_slong_x2():
    fpath = 'data/fdata/reprcodes-x2/14-slong.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(2147483647))
    np.testing.assert_array_equal(
        curves[1][0], np.array(-1))

@pytest.mark.xfail(strict=True)
def test_ushort_x2():
    fpath = 'data/fdata/reprcodes-x2/15-ushort.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(6))
    np.testing.assert_array_equal(
        curves[1][0], np.array(217))

@pytest.mark.xfail(strict=True)
def test_unorm_x2():
    fpath = 'data/fdata/reprcodes-x2/16-unorm.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(32921))
    np.testing.assert_array_equal(
        curves[1][0], np.array(256))

@pytest.mark.xfail(strict=True)
def test_ulong_x2():
    fpath = 'data/fdata/reprcodes-x2/17-ulong.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(1))
    np.testing.assert_array_equal(
        curves[1][0], np.array(4294967143))

@pytest.mark.xfail(strict=True)
def test_uvari_x2():
    fpath = 'data/fdata/reprcodes-x2/18-uvari.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(257))
    np.testing.assert_array_equal(
        curves[1][0], np.array(65536))

@pytest.mark.xfail(strict=True)
def test_ident_x2():
    fpath = 'data/fdata/reprcodes-x2/19-ident.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("VALUE"))
    np.testing.assert_array_equal(
        curves[1][0], np.array("SECOND-VALUE"))

@pytest.mark.xfail(strict=True)
def test_ascii_x2():
    fpath = 'data/fdata/reprcodes-x2/20-ascii.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("Theory of mind"))
    np.testing.assert_array_equal(
        curves[1][0], np.array("this looks terrible"))

@pytest.mark.xfail(strict=True)
def test_dtime_x2():
    fpath = 'data/fdata/reprcodes-x2/21-dtime.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(datetime(1971, 3, 21, 18, 4, 14, 386)))
    np.testing.assert_array_equal(
        curves[1][0], np.array(datetime(1970, 3, 21, 18, 4, 0, 0)))

@pytest.mark.xfail(strict=True)
def test_origin_x2():
    fpath = 'data/fdata/reprcodes-x2/22-origin.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(16777217))
    np.testing.assert_array_equal(
        curves[1][0], np.array(3))

@pytest.mark.xfail(strict=True)
def test_obname_x2():
    fpath = 'data/fdata/reprcodes-x2/23-obname.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array((18, 5, "OBNAME_I")))
    np.testing.assert_array_equal(
        curves[1][0], np.array((18, 5, "OBNAME_K")))

@pytest.mark.xfail(strict=True)
def test_objref_x2():
    fpath = 'data/fdata/reprcodes-x2/24-objref.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(("OBJREF_I", (25, 3, "OBJREF_OBNAME"))))
    np.testing.assert_array_equal(
        curves[1][0], np.array(("OBJREF_I", (25, 4, "OBJREF_OBNAME"))))

@pytest.mark.xfail(strict=True)
def test_attref_x2():
    fpath = 'data/fdata/reprcodes-x2/25-attref.dlis'
    curves = load_curves(fpath)
    ex1_attref = ("FIRST_INDENT", (3, 2, "ATTREF_OBNAME"), "SECOND_INDENT")
    ex2_attref = ("FIRST_INDENT", (9, 2, "ATTREF_OBNAME"), "SECOND_INDENT")
    np.testing.assert_array_equal(
        curves[0][0], np.array(ex1_attref))
    np.testing.assert_array_equal(
        curves[1][0], np.array(ex2_attref))

@pytest.mark.xfail(strict=True)
def test_status_x2():
    fpath = 'data/fdata/reprcodes-x2/26-status.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(True))
    np.testing.assert_array_equal(
        curves[1][0], np.array(False))

@pytest.mark.xfail(strict=True)
def test_units_x2():
    fpath = 'data/fdata/reprcodes-x2/27-units.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array("unit"))
    np.testing.assert_array_equal(
        curves[1][0], np.array("unit2"))


@pytest.mark.xfail(strict=True)
def test_all_reprcodes():
    fpath = 'data/fdata/all-reprcodes.dlis'
    curves = load_curves(fpath)
    np.testing.assert_array_equal(
        curves[0][0], np.array(-1.0))
    np.testing.assert_array_equal(
        curves[0][1], np.array(5.5))
    np.testing.assert_array_equal(
        curves[0][2], np.array((-2, 2)))
    np.testing.assert_array_equal(
        curves[0][3], np.array((117, -13.25, 32444)))
    np.testing.assert_array_equal(
        curves[0][4], np.array(-12))
    np.testing.assert_array_equal(
        curves[0][5], np.array(0.125))
    np.testing.assert_array_equal(
        curves[0][6], np.array(900000000000000.5))
    np.testing.assert_array_equal(
        curves[0][7], np.array((-13.5, -27670)))
    np.testing.assert_array_equal(
        curves[0][8], np.array((6728332223, -45.75, -0.0625)))
    np.testing.assert_array_equal(
        curves[0][9], np.array(complex(93, -14)))
    np.testing.assert_array_equal(
        curves[0][10], np.array(complex(125533556, -4.75)))
    np.testing.assert_array_equal(
        curves[0][11], np.array(89))
    np.testing.assert_array_equal(
        curves[0][12], np.array(-153))
    np.testing.assert_array_equal(
        curves[0][13], np.array(2147483647))
    np.testing.assert_array_equal(
        curves[0][14], np.array(6))
    np.testing.assert_array_equal(
        curves[0][15], np.array(32921))
    np.testing.assert_array_equal(
        curves[0][16], np.array(1))
    np.testing.assert_array_equal(
        curves[0][17], np.array(257))
    np.testing.assert_array_equal(
        curves[0][18], np.array("VALUE"))
    np.testing.assert_array_equal(
        curves[0][19], np.array("ASCII VALUE"))
    np.testing.assert_array_equal(
        curves[0][20], np.array(datetime(1971, 3, 21, 18, 4, 14, 386)))
    np.testing.assert_array_equal(
        curves[0][21], np.array(16777217))
    np.testing.assert_array_equal(
        curves[0][22], np.array((18, 5, "OBNAME_I")))
    np.testing.assert_array_equal(
        curves[0][23], np.array(("OBJREF_I", (25, 3, "OBJREF_OBNAME"))))
    attref = ("FIRST_INDENT", (3, 2, "ATTREF_OBNAME"), "SECOND_INDENT")
    np.testing.assert_array_equal(
        curves[0][24], np.array(attref))
    np.testing.assert_array_equal(
        curves[0][25], np.array(True))
    np.testing.assert_array_equal(
        curves[0][26], np.array("unit"))

@pytest.mark.xfail(strict=True)
def test_two_various_fdata_in_one_iflr():
    fpath = 'data/fdata/two-various-fdata-in-one-iflr.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0],
                          np.array(datetime(1971, 3, 21, 18, 4, 14, 386)))
    np.testing.assert_array_equal(curves[0][1],
                          np.array("VALUE"))
    np.testing.assert_array_equal(curves[0][2],
                          np.array(89))
    np.testing.assert_array_equal(curves[1][0],
                          np.array(datetime(1970, 3, 21, 18, 4, 0, 0)))
    np.testing.assert_array_equal(curves[1][1],
                          np.array("SECOND-VALUE"))
    np.testing.assert_array_equal(curves[1][2],
                          np.array(-89))

@pytest.mark.xfail(strict=True)
def test_inconsequent_framenos_one_frame(assert_log):
    fpath = 'data/fdata/inconsequent-framenos-one-frame.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(False))
    np.testing.assert_array_equal(curves[1][0], np.array(True))

    assert_log("Non-sequential frames")

@pytest.mark.xfail(strict=True)
def test_inconsequent_framenos_two_frames(assert_log):
    fpath = 'data/fdata/inconsequent-framenos-two-frames.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(False))
    np.testing.assert_array_equal(curves[1][0], np.array(True))

    assert_log("Non-sequential frames")

@pytest.mark.xfail(strict=True)
def test_inconsequent_frames_two_framenos_multidata(assert_log):
    fpath = 'data/fdata/inconsequent_framenos_two_frames_multifdata.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(False))
    np.testing.assert_array_equal(curves[1][0], np.array(True))
    np.testing.assert_array_equal(curves[2][0], np.array(True))
    np.testing.assert_array_equal(curves[3][0], np.array(False))

    assert_log("Non-sequential frames")

@pytest.mark.xfail(strict=True)
def test_missing_numbers_frames(assert_log):
    fpath = 'data/fdata/missing-framenos.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(True))
    np.testing.assert_array_equal(curves[1][0], np.array(False))

    assert_log("Missing frames")

@pytest.mark.xfail(strict=True)
def test_duplicate_framenos(assert_log):
    fpath = 'data/fdata/duplicate-framenos.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(True))
    np.testing.assert_array_equal(curves[1][0], np.array(False))

    assert_log("Duplicated frames")


@pytest.mark.xfail(strict=True)
def test_duplicate_framenos_same_frames(assert_log):
    fpath = 'data/fdata/duplicate-framenos-same-frames.dlis'

    curves = load_curves(fpath)
    np.testing.assert_array_equal(curves[0][0], np.array(True))
    np.testing.assert_array_equal(curves[1][0], np.array(True))

    assert_log("Duplicated frames")

def test_fdata_dimension(tmpdir_factory, merge_files_manyLR):
    path = str(tmpdir_factory.mktemp('dimensions').join('fdata-dim.dlis'))
    content = [
        'data/semantic/envelope.dlis.part',
        'data/semantic/file-header2.dlis.part',
        'data/semantic/origin2.dlis.part',
        'data/semantic/channel-dimension.dlis.part',
        'data/semantic/frame-dimension.dlis.part',
        'data/semantic/fdata-dimension.dlis.part',
    ]
    merge_files_manyLR(path, content)

    with dlisio.load(path) as (f, *_):
        frame = f.object('FRAME', 'FRAME-DIMENSION', 11, 0)
        curves = frame.curves()

        assert list(curves[0][0][0])    == [1, 2, 3]
        assert list(curves[0][0][1])    == [4, 5, 6]
        assert list(curves[0][1][0])    == [1, 2]
        assert list(curves[0][1][1])    == [3, 4]
        assert list(curves[0][1][2])    == [5, 6]
        assert list(curves[0][2][0][0]) == [1, 2]
        assert list(curves[0][2][1][1]) == [9, 10]
        assert list(curves[0][2][2][1]) == [15, 16]
        assert list(curves[0][2][3][2]) == [23, 24]
        assert list(curves[0][3][0])    == [1, 2]
        assert list(curves[0][4][0])    == [1]
        assert list(curves[0][4][1])    == [2]
        assert list(curves[0][5][0])    == [1]
        assert list(curves[0][6])       == [1, 2, 3, 4]
