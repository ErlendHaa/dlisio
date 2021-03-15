import pytest
import numpy as np

from dlisio import lis

def test_inforec_components():
    path = 'data/lis/records/inforec_01.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]
    assert len(wellsite.components()) == 16

def test_inforec_table_name():
    path = 'data/lis/records/inforec_01.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured()
    name = wellsite.table_name()

    assert name.component == 'CONS'
    assert name.units     == '    '
    assert name.mnemonic  == 'TYPE'
    assert name.type_nb   == 73

def test_inforec_table_dtype():
    path = 'data/lis/records/inforec_01.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured()
    table = wellsite.table()

    expected_dtype = np.dtype([
        ('MNEM', 'O'),
        ('STAT', 'O'),
        ('PUNI', 'O'),
        ('TUNI', 'O'),
        ('VALU', 'O')
    ])

    assert table.dtype == expected_dtype

def test_inforec_dense_table():
    path = 'data/lis/records/inforec_01.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured()
    table = wellsite.table(simple=True)

    mnem = np.array(['WN  ', 'CN  ', 'SRVC'], dtype='O')
    np.testing.assert_array_equal(table['MNEM'], mnem)

    stat = np.array(['ALLO', 'ALLO', 'ALLO'], dtype='O')
    np.testing.assert_array_equal(table['STAT'], stat)

    puni = np.array(['    ', '    ', '    '], dtype='O')
    np.testing.assert_array_equal(table['PUNI'], puni)

    tuni = np.array(['    ', '    ', '    '], dtype='O')
    np.testing.assert_array_equal(table['TUNI'], tuni)

    valu = np.array(['15/9-F-15 ', 'StatoilHydro', 'Geoservices '], dtype='O')
    np.testing.assert_array_equal(table['VALU'], valu)

def test_inforec_sparse_table():
    path = 'data/lis/records/inforec_02.lis'

    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured()
    table = wellsite.table(simple=True)

    mnem = np.array([-999.25, 'CN  ', 'SRVC'], dtype='O')
    np.testing.assert_array_equal(table['MNEM'], mnem)

    stat = np.array(['ALLO', -999.25, 'ALLO'], dtype='O')
    np.testing.assert_array_equal(table['STAT'], stat)

    puni = np.array(['    ', '    ', '    '], dtype='O')
    np.testing.assert_array_equal(table['PUNI'], puni)

    tuni = np.array(['    ', '    ', '    '], dtype='O')
    np.testing.assert_array_equal(table['TUNI'], tuni)

    valu = np.array(['15/9-F-15 ', 'StatoilHydro', -999.25], dtype='O')
    np.testing.assert_array_equal(table['VALU'], valu)

def test_inforec_unstructured():
    path = 'data/lis/records/inforec_03.lis'

    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured() == False
    assert len(wellsite.components()) == 15

    with pytest.raises(ValueError):
        _ = wellsite.table()

    with pytest.raises(ValueError):
        _ = wellsite.table_name()

def test_inforec_illformed():
    path = 'data/lis/records/inforec_04.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured() == True

    # The table name can still be read
    name = wellsite.table_name()
    assert name.component == 'CONS'

    # The table itself cannot be read
    with pytest.raises(ValueError):
        _ = wellsite.table()

def test_inforec_empty():
    path = 'data/lis/records/inforec_05.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured() == False
    assert len(wellsite.components()) == 0

def test_inforec_empty_table():
    path = 'data/lis/records/inforec_06.lis'
    f, = lis.load(path)
    wellsite = f.wellsite_data()[0]

    assert wellsite.isstructured() == True
    assert len(wellsite.components()) == 1

    name = wellsite.table_name()
    assert name.component == 'CONS'

    np.testing.assert_array_equal(wellsite.table(), np.empty(0))
