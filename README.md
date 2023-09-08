<!--
SPDX-FileCopyrightText: 2016-2021 Comcast Cable Communications Management, LLC
SPDX-License-Identifier: Apache-2.0
-->
We are working on a replacement, but it will be authored in Go.  This repo is archived.


# xmidt-agent

<!--
[![Build Status](https://github.com/xmidt-org/xmidt-agent/workflows/CI/badge.svg)](https://github.com/xmidt-org/xmidt-agent/actions)
[![codecov.io](http://codecov.io/github/xmidt-org/xmidt-agent/coverage.svg?branch=main)](http://codecov.io/github/xmidt-org/xmidt-agent?branch=main)
[![Coverity](https://img.shields.io/coverity/scan/23259.svg)](https://scan.coverity.com/projects/xmidt-org-xmidt-agent)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=xmidt-org_xmidt-agent&metric=alert_status)](https://sonarcloud.io/dashboard?id=xmidt-org_xmidt-agent)
[![Language Grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/xmidt-org/xmidt-agent.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/xmidt-org/xmidt-agent/context:cpp)
[![Apache V2 License](http://img.shields.io/badge/license-Apache%20V2-blue.svg)](https://github.com/xmidt-org/xmidt-agent/blob/main/LICENSE)
[![GitHub release](https://img.shields.io/github/release/xmidt-org/xmidt-agent.svg)](CHANGELOG.md)
-->


The new xmidt client agent.

XMiDT is a highly scalable, highly available, generic message routing system.

# Building and Testing Instructions

```
meson setup --warnlevel 3 --werror build
cd build
ninja all test coverage
firefox meson-logs/coveragereport/index.html
```
