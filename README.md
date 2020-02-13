AmptekHardwareInterface
=======================

Communication interface for Amptek PX5 as a new backend for a Tango Device Server.
This interface will be designed to work with both, USB and UDP connection. It is based on the original AmptekPX5 Tango Device Server.

Current Plan is, to strip the connection-dependent part to seperate comm handlers for USB and UDP and use common code for everything except receiving and sending of the packets.
The Tango Server will the be implemented using python and SWIG. This is based on the much more modern design of Tango device servers in python resulting in a better readability and development. No need for code generators!

Install
-------
Run

    python setup.py install


TODOs
------
- propagde the device state properly up to tango
- verify correct error handling
- add comments, docs

