ssl_stat_js
===========

Node.js C++ Language Extension To Get SSL Certificate Information From URL

Install Dependencies (Ubuntu)
=============================
::

    apt install libcurl3 libcurl-openssl1.0-dev

Installing Through npm
======================
::

	npm i ssl_stat

Sample Usage
============
::

    var ssl_stat = require("ssl_stat");
    ssl_stat.check("https://example.com");
