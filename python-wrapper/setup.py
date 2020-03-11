"""
cabp-wrapper: Basic wrapper for libCABP Communication Area Based Positioning.
"""

from setuptools import setup, find_packages
from os.path import join as pjoin

doclines = __doc__.split("\n")

setup(name='cabp_wrapper',
      version="0.0.0",
      description='Communication Area Based Positioning.',
      long_description='\n'.join(doclines[2:]),
      url='http://github.com/raidoz/cabp',
      author='Raido Pahtma',
      author_email='raido@author_email',
      license='TBD',
      platforms=['any'],
      packages=find_packages(),
      install_requires=["utm"],
      test_suite='nose.collector',
      tests_require=['nose'],
      zip_safe=False)
