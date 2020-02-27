"""Configuration classes for handling directories etc"""
import os


class TestingConfigurations(object):
    """parameters in this configuration might change"""
    TEST_DATA_ROOT = os.environ['SYSTEMC_CLANG_BUILD_DIR'] + "../systemc-clang/examples/"

    @property
    def extra_header_folders(self):
        """acessor for header folders"""
        # SystemC Clang build llnl-examples
        return self.header_folders

    @extra_header_folders.setter
    def set_extra_header_folders(self, value):
        self._extra_header_folders = value

    @property
    def positional_arguments(self):
        """positional arguments for the systemc-clang command"""
        headers = []
        extra_header_folders = self.extra_header_folders
        for header in extra_header_folders:
            headers.extend(["-I", header])
        return headers

    def get_module_name(self, basename):
        return self.root_folder + '/' + basename

    def get_golden_sexp_name(self, basename):
        return self.golden_folder + '/' + basename

    def get_golden_verilog_name(self, basename):
        return self.golden_folder + '/' + basename

    def __init__(self, root_folder, golden_folder, header_folders):
        self.header_folders = header_folders
        self.root_folder = root_folder
        self.golden_folder = golden_folder


class LLNLExampleTestingConfigurations(TestingConfigurations):
    """parameters for LLNL examples"""

    def __init__(self, header_folders=None):
        if header_folders is None:
            header_folders = []
        this_folders = header_folders + [
            '{}/llnl-examples/'.format(TestingConfigurations.TEST_DATA_ROOT)
        ]
        root_folder =  os.environ['SYSTEMC_CLANG_BUILD_DIR'] + '/' + 'tests/data/verilog-conversion/llnl-examples/'
        golden_folder = root_folder + 'golden/'
        super(LLNLExampleTestingConfigurations, self).__init__(root_folder, golden_folder, this_folders)


class ExampleTestingConfigurations(TestingConfigurations):
    """parameters for ex_* examples"""

    def __init__(self, ex_id, header_folders=None):
        self.ex_id = ex_id
        if header_folders is None:
            header_folders = []
        this_folders = header_folders + [
            '{}/ex_{}/'.format(TestingConfigurations.TEST_DATA_ROOT, ex_id)
        ]
        # TODO: remove this special case for dependency crossing folder boundaries
        if ex_id == 13:
            this_folders.append('{}/ex_12/'.format(TestingConfigurations.TEST_DATA_ROOT))
        root_folder = '{}/ex_{}/'.format(
            TestingConfigurations.TEST_DATA_ROOT,
            ex_id
        )
        golden_folder = '{}/tests/data/verilog-conversion/ex_{}/'\
                        'golden/'.format(
            os.environ['SYSTEMC_CLANG_BUILD_DIR'], 
            ex_id
        )
        super(ExampleTestingConfigurations, self).__init__(
            root_folder, 
            golden_folder, 
            this_folders
        )

class SanityTestingConfigurations(TestingConfigurations):
    """parameters for sanity test"""
    def __init__(self, header_folders=None):
        if header_folders is None:
            header_folders = []
        root_folder =  os.environ['SYSTEMC_CLANG_BUILD_DIR'] + '/' + 'tests/data/verilog-conversion/sanity/'
        golden_folder = root_folder + 'golden/'
        super(SanityTestingConfigurations, self).__init__(root_folder, golden_folder, header_folders)
