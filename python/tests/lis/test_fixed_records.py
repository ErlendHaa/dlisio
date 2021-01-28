import pytest

from dlisio import lis

def test_file_header():
    with lis.load('data/lis/MUD_LOG_1.LIS') as (_, f, *tail):
        header = f.header()

        assert header.file_name           == 'LIS1  .001'
        assert header.service_sublvl_name == '      '
        assert header.version_number      == '        '
        assert header.date_of_generation  == '        '
        assert header.max_pr_length       == ' 1024'
        assert header.file_type           == '  '
        assert header.prev_file_name      == '          '

        with pytest.raises(RuntimeError):
            _ = header.next_file_name

def test_file_header():
    with lis.load('data/lis/MUD_LOG_1.LIS') as (_, f, *tail):
        trailer = f.trailer()

        assert trailer.file_name           == 'LIS1  .001'
        assert trailer.service_sublvl_name == '      '
        assert trailer.version_number      == '        '
        assert trailer.date_of_generation  == '        '
        assert trailer.max_pr_length       == ' 1024'
        assert trailer.file_type           == '  '
        assert trailer.next_file_name      == '          '

        with pytest.raises(RuntimeError):
            _ = trailer.prev_file_name

