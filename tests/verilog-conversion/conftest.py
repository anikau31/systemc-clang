"""Fixtures for testing"""
import os
import pytest
from util.conf import LLNLExampleTestingConfigurations
from util.conf import ExampleTestingConfigurations
from util.conf import TestingConfigurations
import driver as drv


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

@pytest.fixture()
def tool_output(pytestconfig):
    return pytestconfig.getoption("tool_output")

def pytest_addoption(parser):
    """add options for controlling the running of tests"""
    # whether output the clang output
    parser.addoption("--tool-output", action="store_true", default=False)

