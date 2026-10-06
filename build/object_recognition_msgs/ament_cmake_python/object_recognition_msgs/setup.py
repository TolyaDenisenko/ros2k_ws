from setuptools import find_packages
from setuptools import setup

setup(
    name='object_recognition_msgs',
    version='2.0.0',
    packages=find_packages(
        include=('object_recognition_msgs', 'object_recognition_msgs.*')),
)
