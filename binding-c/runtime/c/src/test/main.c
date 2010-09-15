/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#ifdef _VLD_H_
#include "vld.h"
#endif

#include "test_all.h"
#include <stdlib.h>
#include <string.h>
#include "CUnit.h"
#include "Basic.h"
#include "Automated.h"
#include "Console.h"
#include <stdio.h>
#include <apr.h>
#include "apr_time.h"

void run_console(int argc, char** args) {
    CU_console_run_tests();
}

void run_automatic(int argc, char** args) {
  CU_set_output_filename("alltests");
  CU_automated_run_tests();
}

void run_selected(int argc, char** args) {
  int i;
  CU_pTestRegistry reg;
  CU_pSuite suiteToRun;
  reg = CU_get_registry();
  for (i=1; i<argc; i++) {
    if (args[i][0] != '-') {
      suiteToRun = CU_get_suite_by_name(args[i], reg);
      if (suiteToRun) {
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_suite(suiteToRun);
      } else {
        printf("suite '%s' not found\n", args[i]);
      }
    }
  }
}

void run_default(int arg, char** args) {
  char c;
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  printf("press <enter> to exit.");
  scanf("%c", &c);
}

void (*runner)(int argc, char** args);

void parseOptions(int argc, char** args) {
  runner = run_default;
  if (argc >= 2) {
    char* parameter = args[1];
    if (strcmp(parameter, "-c") == 0) {
	runner = run_console;
    } else if (strcmp(parameter, "-a") == 0) {
	runner = run_automatic;
    } else if (strcmp(parameter, "-s") == 0) {
	runner = run_selected;
    }
  }
}

int main(int argc, char** argv) {
    int i=0;
    CU_ErrorCode rv = 0;

    parseOptions(argc, argv);

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return 1;
    }

    while (etch_testlist[i] != 0) {
      (void)etch_testlist[i++]();
    }

    runner(argc, argv);

    CU_cleanup_registry();
    rv = CU_get_error();

    return rv;
}
