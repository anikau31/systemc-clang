"""Fixtures for testing"""
import os
import glob
import pytest
from util.conf import LLNLExampleTestingConfigurations
from util.conf import ExampleTestingConfigurations
from util.conf import TestingConfigurations
from util.conf import SanityTestingConfigurations
from util.conf import CustomTestingConfigurations
import driver as drv

def pytest_configure(config):
    # register an additional marker
    config.addinivalue_line(
        "markers", "verilog: cpp to verilog tests"
    )
    pytest.sc_log = open('log.csv', 'w')

@pytest.fixture
def llnldriver():
    """fixture for llnl example driver"""
    conf = LLNLExampleTestingConfigurations()
    llnl_driver = drv.SystemCClangDriver(conf)
    return llnl_driver


# @pytest.fixture(params=[13])
@pytest.fixture(params=list(range(1, 11)) + list(range(12, 16)))
def exdriver(request):
    """fixture for ex_* examples driver"""
    conf = ExampleTestingConfigurations(request.param)
    ex_driver = drv.SystemCClangDriver(conf)
    return ex_driver


@pytest.fixture
def sanitydriver():
    """fixture for sanity test driver"""
    conf = SanityTestingConfigurations()
    sanity_driver = drv.SystemCClangDriver(conf)
    return sanity_driver


@pytest.fixture
def testfolderdriver():
    """fixture for running tests in the test folder of
    the systemc-clang"""
    root_folder = os.environ['SYSTEMC_CLANG_BUILD_DIR'] + '../systemc-clang/tests/'
    conf = TestingConfigurations(
            root_folder=root_folder, 
            golden_folder=[],
            header_folders=[]
    )
    testfolder_driver = drv.SystemCClangDriver(conf)
    return testfolder_driver


def get_custom_tests():
    """helper function for collecting custom tests"""
    root_folder = os.environ['SYSTEMC_CLANG_BUILD_DIR'] + '/' + 'tests/data/verilog-conversion-custom/'
    folder_list = glob.glob(root_folder + '*')
    print(folder_list)
    for f in folder_list:
        assert os.path.isdir(f), "{} is not a directory".format(f)
    return list(map(lambda x: os.path.basename(x), folder_list))


@pytest.fixture(params=get_custom_tests())
def customdriver(request):
    """fixture for custom test driver"""
    conf = CustomTestingConfigurations(request.param)
    custom_driver = drv.SystemCClangDriver(conf)
    return custom_driver


@pytest.fixture()
def tool_output(pytestconfig):
    return pytestconfig.getoption("tool_output")

def pytest_addoption(parser):
    """add options for controlling the running of tests"""
    # whether output the clang output
    parser.addoption("--tool-output", action="store_true", default=False)

