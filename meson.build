# Meson build file
#
# SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

project('xmidt-agent', 'c',
        version: '0.0.1',
        license: ['Apache-2.0', 'MIT'],
        default_options: ['c_std=c99',
                          'b_coverage=true'])

# We want strict flags
warning_level = 3   # -Wall
werror = true

cc = meson.get_compiler('c')

################################################################################
# Generate the version header file
################################################################################
subdir('src')

################################################################################
# Define the dependencies
################################################################################
cjwt_dep             = dependency('cjwt',          version: '>=2.0.1', required: get_option('dns-txt-token'))
curl_dep             = dependency('libcurl',       version: '>=7.50.2')
curlws_dep           = dependency('curlws',        version: '>=1.0.4')
cutils_dep           = dependency('cutils',        version: '>=1.0.0')
libcjson_dep         = dependency('libcjson',      version: '>=1.7.14', fallback: ['cjson'])
libtrower_base64_dep = dependency('trower-base64', version: '>=1.2.7')
ludocode_mpack_dep   = dependency('mpack',         version: '>=1.0', fallback: ['ludocode-mpack', 'ludocode_mpack_dep'])
otelc_dep            = dependency('otelc',         version: '>=0.0.0')
uuid_dep             = dependency('uuid')
wrpc_dep             = dependency('wrp-c',         version: '>=2.0.0')

resolv_dep = cc.find_library('resolv', required: get_option('dns-txt-token'))

all_dep = [curl_dep,
           curlws_dep,
           cutils_dep,
           libcjson_dep,
           libtrower_base64_dep,
           ludocode_mpack_dep,
           otelc_dep,
           uuid_dep,
           wrpc_dep]

if get_option('dns-txt-token')
  all_dep += [cjwt_dep, resolv_dep]
endif

################################################################################
# Define the main program
################################################################################

sources = [ 'src/codes.c',
            'src/log.c',
            'src/main.c' ]

if get_option('auth-token')
  sources += [ 'src/auth_token.c' ]
endif
if get_option('dns-txt-token')
  sources += [ 'src/dns_txt.c' ]
endif

executable(meson.project_name(),
           sources,
           dependencies: all_dep,
           install: true)

################################################################################
# Define the examples
################################################################################
if get_option('examples')

  if get_option('auth-token')
    executable('auth_fetch_cli',
               [ 'examples/auth-fetch-cli/cli.c',
                 'src/auth_token.c' ],
                 dependencies: [curl_dep, uuid_dep])
  endif

  if get_option('dns-txt-token')
    executable('dns_token_cli',
               [ 'examples/dns-token-cli/cli.c',
                 'src/codes.c',
                 'src/dns_txt.c'],
                 dependencies: all_dep)
  endif
endif

################################################################################
# Define the tests
################################################################################

if get_option('tests')
  test_args = ['-fprofile-arcs', '-g', '-ftest-coverage', '-O0']
  cunit_dep = dependency('cunit')

  tests = {
    'test_auth_token': {
      'srcs': [ 'tests/test_auth_token.c',
                'src/auth_token.c'],
      'deps': [ curl_dep, cutils_dep ],
      'opt': 'auth-token',
    },
    'test_dns_txt': {
      'srcs': [ 'tests/test_dns_txt.c',
                'src/codes.c',
                'src/dns_txt.c'],
      'deps': [ all_dep ],
      'opt': 'dns-txt-token',
    },
    'test_logs': {
      'srcs': [ 'tests/test_log.c',
                'src/log.c'],
      'deps': [ all_dep ],
    },
  }

  foreach test, vals : tests
    if 'opt' in vals and not get_option(vals['opt'])
      message('Skipping test: \u001b[1m'+test+'\u001b[0m ('+vals['opt']+' not enabled)')
    else
      deps = [cunit_dep]
      if 'deps' in vals
      deps += vals['deps']
      endif
      test(test,
           executable(test, vals['srcs'],
                      dependencies: deps,
                      install: false,
                      link_args: test_args))
    endif
  endforeach

  add_test_setup('valgrind',
                 is_default: true,
                 exe_wrapper: [ 'valgrind',
                                '--leak-check=full',
                                '--error-exitcode=1' ],
                 timeout_multiplier: 2)
endif

################################################################################
# Add the license check target
################################################################################

reuse_bin = find_program('reuse', required: false)
if reuse_bin.found()
  custom_target('license',
                command: [reuse_bin, 'lint'],
                build_always_stale: true,
                output: ['license'])
endif
