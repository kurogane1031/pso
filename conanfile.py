from conans import ConanFile

class ConanPackage(ConanFile):
    name = 'PSO'
    version = "0.1.5"

    generators = 'cmake_find_package'

    requires = [
        ('toml11/3.6.0'),
    ]

