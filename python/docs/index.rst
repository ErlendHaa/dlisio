.. image:: ../../dlisio-logo.svg
   :width: 400
   :alt: dlisio logo
   :align: center

Welcome to dlisio. dlisio is a python package for reading Digital Log
Interchange Standard (DLIS) v1. Version 2 exists, and has been around for quite
a while, but it is our understanding that most dlis files out there are still
version 1. Hence dlisio's focus is put on version 1 [1]_, for now.

As of version 0.3.0, dlisio is extended to also read Log Information Standard 79
(LIS79) [2]_. An extended version of the LIS79 standard, called LIS84/Enhanced LIS
exists, but this version is currently not supported by dlisio.

.. warning::
   The LIS79 reader should be used with caution. It is experimental and
   lacks thorough testing and real world experience. Early adoptors are
   encuraged to report any issues or bug on the issue tracker on GitHub [3]_

Before you get started we recommended that you familiarize yourself with some
basic concepts of the DLIS- and LIS file formats. These are non-trivial formats
and some knowledge about them is required to work effectively with them. A good place to start is
the user guides: :ref:`DLIS User Guide` and :ref:`LIS User Guide`.

.. note::
   Please note that dlisio is still in alpha, so expect breaking changes between
   versions.

.. [1] API RP66 v1, http://w3.energistics.org/RP66/V1/Toc/main.html
.. [2] LIS79, http://w3.energistics.org/LIS/lis-79.pdf
.. [3] Issue Tracker, https://github.com/equinor/dlisio/issues

Installation
============

dlisio can be installed with `pip <https://pip.pypa.io>`_

.. code-block:: bash

    $ python3 -m pip install dlisio

Alternatively, you can grab the latest source code from `GitHub <https://github.com/equinor/dlisio>`_.

About the project
=================

dlisio attempts to abstract away a lot of the pain of LIS and DLIS and give
access to the data in a simple and easy-to-use manner. It gives the user the
ability to work with these files without having to know *all* the details of the
standard itself. Its main focus is making the data accessible while putting
little assumptions on how the data is to be used.

dlisio is written and maintained by Equinor ASA as a free, simple, easy-to-use
library to read well logs that can be tailored to our needs, and as a
contribution to the open-source community.

.. toctree::
   :maxdepth: 1

   changelog

.. toctree::
   :caption: Log Interchange Standard
   :name: LIS

   lis/specification
   lis/userguide
   lis/api

.. toctree::
   :caption: Digital Log Interchange Standard
   :name: rp66
   :maxdepth: 3

   dlis/specification
   dlis/userguide
   dlis/metadata
   dlis/curves
   dlis/api
   dlis/vendors

.. toctree::
   :caption: Common
   :name: common
   :maxdepth: 3

   common-api

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

