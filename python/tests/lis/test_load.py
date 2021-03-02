from dlisio import lis, core

""" Test different file configurations/layouts w.r.t. TapeImageFormat and
padding. All test/file contains the same LIS data, but with different
configurations of tapemarks and padding.

Please refer to data/lis/layouts/README.rst for what each file is designed to
test.
"""

def assert_load_correctly(fpath):
    with lis.load(fpath) as files:
        assert len(files) == 2

        for f in files:
            assert len(f.explicits()) == 2
            assert f.reel.rawheader.info.type  == core.lis_rectype.reel_header
            assert f.reel.rawtrailer.info.type == core.lis_rectype.reel_trailer
            assert f.tape.rawheader.info.type  == core.lis_rectype.tape_header
            assert f.tape.rawtrailer.info.type == core.lis_rectype.tape_trailer

def test_layout_01():
    fpath = 'data/lis/layouts/layout_01.lis'
    assert_load_correctly(fpath)

def test_layout_02():
    fpath = 'data/lis/layouts/layout_02.lis'
    assert_load_correctly(fpath)

def test_layout_03():
    fpath = 'data/lis/layouts/layout_03.lis'
    assert_load_correctly(fpath)

def test_layout_04():
    fpath = 'data/lis/layouts/layout_04.lis'
    assert_load_correctly(fpath)

def test_layout_05():
    fpath = 'data/lis/layouts/layout_05.lis'
    assert_load_correctly(fpath)

def test_layout_06():
    fpath = 'data/lis/layouts/layout_06.lis'
    assert_load_correctly(fpath)

def test_layout_07():
    fpath = 'data/lis/layouts/layout_07.lis'
    assert_load_correctly(fpath)

def test_layout_08():
    fpath = 'data/lis/layouts/layout_08.lis'
    assert_load_correctly(fpath)

def test_layout_09():
    fpath = 'data/lis/layouts/layout_09.lis'
    assert_load_correctly(fpath)

def test_layout_10():
    fpath = 'data/lis/layouts/layout_10.lis'
    assert_load_correctly(fpath)

def test_layout_11():
    fpath = 'data/lis/layouts/layout_11.lis'
    assert_load_correctly(fpath)

def test_layout_12():
    fpath = 'data/lis/layouts/layout_12.lis'
    assert_load_correctly(fpath)

def test_broken_01():
    fpath = 'data/lis/layouts/broken_01.lis'
    assert_load_correctly(fpath)
