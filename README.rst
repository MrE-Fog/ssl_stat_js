ssl_stat_js
===========

Node.js C++ Language Extension To Get SSL Certificate Information From URL

Install Dependency (Ubuntu)
===========================
::

    apt install libcurl4-openssl-dev

Installing Through npm
======================
::

	npm i ssl_stat

Sample Usage
============
::

    var ssl_stat = require("ssl_stat");
    ssl_stat.check("https://example.com");