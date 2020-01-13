"""Fixtures for testing"""
import pytest
from util.conf import LLNLExampleTestingConfigurations
import driver as drv


@pytest.fixture
def llnldriver():
    """fixture for llnl example driver"""
    conf = LLNLExampleTestingConfigurations()
    llnl_driver = drv.SystemCClangDriver(conf)
    return llnl_driver


def pytest_addoption(parser):
    parser.addoption("--tool-output", action="store_true")


def pytest_generate_tests(metafunc):
    # This is called for every test. Only get/set command line arguments
    # if the argument is specified in the list of test "fixturenames".
    if 'tool_output' in metafunc.fixturenames:
        if metafunc.config.getoption("tool_output"):
            verbose = True
        else:
            verbose = False
        metafunc.parametrize("tool_output", [verbose])
