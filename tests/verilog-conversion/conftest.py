"""Fixtures for testing"""
import pytest
from util.conf import LLNLExampleTestingConfigurations
from util.conf import ExampleTestingConfigurations
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


def pytest_addoption(parser):
    """add options for controlling the running of tests"""
    # whether output the clang output
    parser.addoption("--tool-output", action="store_true")


def pytest_generate_tests(metafunc):
    """augmenting parameters to tests"""
    # This is called for every test. Only get/set command line arguments
    # if the argument is specified in the list of test "fixturenames".
    if 'tool_output' in metafunc.fixturenames:
        verbose = bool(metafunc.config.getoption("tool_output"))
        metafunc.parametrize("tool_output", [verbose])
