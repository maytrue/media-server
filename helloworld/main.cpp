
//
//  main.cpp
//  Denoise
//
//  Created by guowei on 08/06/2017.
//  Copyright © 2017 MOMO. All rights reserved.
//

#include <iostream>
#include <uv.h>
#include <stdlib.h>
#include <stdio.h>


int main() {

  uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  uv_loop_init(loop);

  uv_run(loop, UV_RUN_DEFAULT);

  uv_loop_close(loop);
  free(loop);


  return 0;
}