#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "defs.h"


/*
 * CS4400
 * HW3
 * Author: Taku Sakikawa
 */

/******************************************************
 * PINWHEEL KERNEL
 *
 * Your different versions of the pinwheel kernel go here
 ******************************************************/

/*
 * naive_pinwheel - The naive baseline version of pinwheel
 */
char naive_pinwheel_descr[] = "naive_pinwheel: baseline implementation";
void naive_pinwheel(pixel *src, pixel *dest)
{
  int i, j;

 for (i = 0; i < src->dim; i++)
   for (j = 0; j < src->dim; j++) {
     /* Check whether we're in the diamond region */
     if ((fabs(i + 0.5 - src->dim/2) + fabs(j + 0.5 - src->dim/2)) < src->dim/2) {
       /* In diamond region, so rotate and grayscale */
       int s_idx = RIDX(i, j, src->dim);
       int d_idx = RIDX(src->dim - j - 1, i, src->dim);
       dest[d_idx].red = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
       dest[d_idx].green = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
       dest[d_idx].blue = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
     } else {
       /* Not in diamond region, so keep the same */
       int s_idx = RIDX(i, j, src->dim);
       int d_idx = RIDX(i, j, src->dim);
       dest[d_idx] = src[s_idx];
     }
   }
}


// speed up about 1.6
char p4dec[] = "p4";
void p4(pixel *src, pixel *dest)
{
  int i, j,ii,jj;
  int dim = src->dim;
  int halfdim = dim/2;
  int color2 ;
  double halfdim5 = 0.5 - halfdim;
  double ihalfdim5;
  double n;
  int s_idx = 1;
  int d_idx ;



  for(i = 0 ; i < dim ; i +=16)
  {
    for(j = 0 ; j < dim ; j +=16)
    {
      for(ii = i; ii < i + 16; ii++)
      {
        ihalfdim5 = fabs(ii + halfdim5);

        n = halfdim - ihalfdim5;
        s_idx = RIDX(ii, j, dim);
        d_idx = RIDX(dim - j - 1, ii, dim);


        for(jj = j; jj < j + 16; jj++)
        {
          pixel * sp = &src[s_idx];
          if (( fabs(jj + halfdim5)) < n) {
            /* In diamond region, so rotate and grayscale */
            color2 = ((int)sp->red + sp->green + sp->blue) / 3;
            pixel * dp = &dest[d_idx];
            dp->red = dp->blue = dp->green = color2;

          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx ++ ;
          d_idx -=dim;



          jj++;
          if (( fabs(jj + halfdim5)) < n) {
            /* In diamond region, so rotate and grayscale */

            // int d_idx = RIDX(dim - jj - 1, ii, dim);

            pixel * sp = &src[s_idx];
            color2 = ((int)sp->red + sp->green + sp->blue) / 3;

            pixel * dp = &dest[d_idx];
            dp->red = dp->blue = dp->green = color2;

          } else {
            /* Not in diamond region, so keep the same */
            // int s_idx = RIDX(ii, jj, dim);
            dest[s_idx] = src[s_idx];
          }
          s_idx ++ ;
          d_idx -=dim;

        } // for jj
      }// for ii
    } // for j
  } //for i
}

// speed up about 2.0 ~ 2.1
char p3dec[] = "pinwheel: p3";
void p3(pixel *src, pixel *dest)
{
  int i, j;
  int dim =  src->dim;
  int halfdim = dim/2;
  double halfdim5 = 0.5 - halfdim;
  double jhalfdim5;
  int n;
  double ihalfdim5;
  double ihalfdim5_copy = halfdim5;

  int s_idx=1;
  int d_idx = dim * dim - dim +1 ;//RIDX(dim - 0 - 1, 0, dim);
  int d_copy = d_idx;

  int color2;

 for (i = 0; i < dim; i++){
  ihalfdim5 = fabs(ihalfdim5_copy);
  ihalfdim5_copy++;
   n = halfdim - ihalfdim5;
   jhalfdim5 = halfdim5;

   for (j = 0; j < dim; j+=4) {

     /* Check whether we're in the diamond region */
     if (fabs(jhalfdim5) < n) {
       /* In diamond region, so rotate and grayscale */
       pixel * sp = &src[s_idx];
        color2 = ((int)sp->red + sp->green + sp->blue) / 3;
       pixel * dp = &dest[d_idx];
       dp->red = dp->blue = dp->green=  color2;
     } else {
       /* Not in diamond region, so keep the same */
       dest[s_idx] = src[s_idx];
     }
     s_idx++;
     d_idx-=dim;

     if ((fabs(++jhalfdim5)) < n) {
       // /* In diamond region, so rotate and grayscale */
       pixel * sp = &src[s_idx];
        color2 = ((int)sp->red + sp->green + sp->blue) / 3;
       pixel * dp = &dest[d_idx];
       dp->red = dp->blue = dp->green=  color2;
     } else {
       /* Not in diamond region, so keep the same */
       dest[s_idx] = src[s_idx];

     }
     s_idx++;
     d_idx-=dim;

     if ((fabs(++jhalfdim5)) < n) {
       // /* In diamond region, so rotate and grayscale */
       pixel * sp = &src[s_idx];
        color2 = ((int)sp->red + sp->green + sp->blue) / 3;
       pixel * dp = &dest[d_idx];
       dp->red = dp->blue = dp->green=  color2;
     } else {
       /* Not in diamond region, so keep the same */
       dest[s_idx] = src[s_idx];

     }
     s_idx++;
     d_idx-=dim;

     if ((fabs(++jhalfdim5)) < n) {
       // /* In diamond region, so rotate and grayscale */
       pixel * sp = &src[s_idx];
        color2 = ((int)sp->red + sp->green + sp->blue) / 3;
       pixel * dp = &dest[d_idx];
       dp->red = dp->blue = dp->green=  color2;
     } else {
       /* Not in diamond region, so keep the same */
       dest[s_idx] = src[s_idx];

     }
     jhalfdim5++;
     s_idx++;
     d_idx-=dim;

   }
   d_idx= ++d_copy;
 }
 // s_idx+=dim;
}


//speed up about 2.7 ~ 2.8
char real_one_dec[] = "real one";
void real_one(pixel *src, pixel *dest)
{
  int i, j,ii,jj;
  int W = 8;
  int dim = src->dim;
  int halfdim = dim/2;
  double hd5 = 1.0/2 - halfdim;

  for (i = 0; i < dim; i += W)
  {
    for (j = 0; j < dim; j += W)
    {
      for (ii = i; ii < i+W; ii++)
      {
        double fabi = fabs(ii + hd5);
        int d_idx = RIDX(src->dim - j - 1, ii, dim);
        int s_idx = RIDX(ii, j, dim);
        for (jj = j; jj < j+W; jj+=W)
        {

          if ((fabs(jj + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;


          if ((fabs(jj+1 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;



          if ((fabs(jj+2 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;

          if ((fabs(jj+3 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;

          if ((fabs(jj+4 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;

          if ((fabs(jj+5 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;

          if ((fabs(jj+6 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;

          if ((fabs(jj+7 + hd5)) < halfdim-fabi) {
            /* In diamond region, so rotate and grayscale */
            pixel *p = &src[s_idx];
            pixel *d = &dest[d_idx];
            d->red = d->blue = d->green = ((int)p->red + p->green + p->blue) / 3;
          } else {
            /* Not in diamond region, so keep the same */
            dest[s_idx] = src[s_idx];
          }
          s_idx++;
          d_idx-=dim;
        }
      }
    }
  }
}



/*
 * pinwheel - Your current working version of pinwheel
 * IMPORTANT: This is the version you will be graded on
 */
char pinwheel_descr[] = "pinwheel: Current working version";
void pinwheel(pixel *src, pixel *dest)
{
  real_one(src, dest);
}




/*********************************************************************
 * register_pinwheel_functions - Register all of your different versions
 *     of the pinwheel kernel with the driver by calling the
 *     add_pinwheel_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_pinwheel_functions() {
  add_pinwheel_function(&pinwheel, pinwheel_descr);
  add_pinwheel_function(&naive_pinwheel, naive_pinwheel_descr);
}


/***************************************************************
 * GLOW KERNEL
 *
 * Starts with various typedefs and helper functions for the glow
 * function, and you may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
  int red;
  int green;
  int blue;
} pixel_sum;

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum(pixel_sum *sum)
{
  sum->red = sum->green = sum->blue = 0;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_weighted_sum(pixel_sum *sum, pixel p, double weight)
{
  sum->red += (int) p.red * weight;
  sum->green += (int) p.green * weight;
  sum->blue += (int) p.blue * weight;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
  current_pixel->red = (unsigned short)sum.red;
  current_pixel->green = (unsigned short)sum.green;
  current_pixel->blue = (unsigned short)sum.blue;
}

/*
 * weighted_combo - Returns new pixel value at (i,j)
 */
static pixel weighted_combo(int dim, int i, int j, pixel *src)
{
  int ii, jj;
  pixel_sum sum;
  pixel current_pixel;
  double weights[3][3] = { { 0.16, 0.00, 0.16 },
                           { 0.00, 0.30, 0.00 },
                           { 0.16, 0.00, 0.16 } };

  initialize_pixel_sum(&sum);
  for (ii=-1; ii < 2; ii++)
    for (jj=-1; jj < 2; jj++)
      if ((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
        accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii+1][jj+1]);

  assign_sum_to_pixel(&current_pixel, sum);

  return current_pixel;
}

/******************************************************
 * Your different versions of the glow kernel go here
 ******************************************************/

/*
 * naive_glow - The naive baseline version of glow
 */
char naive_glow_descr[] = "naive_glow: baseline implementation";
void naive_glow(pixel *src, pixel *dst)
{
  int i, j;

  for (i = 0; i < src->dim; i++)
    for (j = 0; j < src->dim; j++)
      dst[RIDX(i, j, src->dim)] = weighted_combo(src->dim, i, j, src);

}


/*
 * glow - version 2
 */
char g2dec[] = "real one";
void g2(pixel *src, pixel *dst)
{
  int i, j;

  int dim = src->dim;

  for( i =0; i< dim; i++)
  {
    for(j = 0; j < dim ; j++)
    {
      // int i, j;
      int ii=-1;
      int jj=-1;
      pixel_sum _sum;
      pixel_sum *sum = &_sum;
      pixel current_pixel;
      initialize_pixel_sum(&_sum);

      // when (ii jj)= (-1 -1)
      if((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
      {
        pixel p = src[RIDX(i+ii,j+jj,dim)];
        sum->red += (int) (p.red * 4/25);
        sum->green += (int) (p.green * 4/25);
        sum->blue += (int) (p.blue * 4/25);
      }
      //when  (ii jj)= (0 -1) -> weights is 0 ---- ignore

      //when  (ii jj)= (1 -1) -> weights is 0.16
      ii+=2;
      if((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
      {
        pixel p = src[RIDX(i+ii,j+jj,dim)];
        sum->red += (int) (p.red * 4/25);
        sum->green += (int) (p.green * 4/25);
        sum->blue += (int) (p.blue * 4/25);
      }

      //when  (ii jj)= (0 0) -> weights is 0.3
      ii--;
      jj++;
      if((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
      {
        pixel p = src[RIDX(i+ii,j+jj,dim)];
        sum->red += (int) (p.red * 3/10);
        sum->green += (int) (p.green * 3/10);
        sum->blue += (int) (p.blue * 3/10);
      }

      //when  (ii jj)= (-1 1) -> weights is 0.16
      ii--;
      jj++;
      if((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
      {
        pixel p = src[RIDX(i+ii,j+jj,dim)];
        sum->red += (int) (p.red * 4/25);
        sum->green += (int) (p.green * 4/25);
        sum->blue += (int) (p.blue * 4/25);
      }

      //when  (ii jj)= (1 1) -> weights is 0.16
      ii+=2;
      if((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
      {
        pixel p = src[RIDX(i+ii,j+jj,dim)];
        sum->red += (int) (p.red * 4/25);
        sum->green += (int) (p.green * 4/25);
        sum->blue += (int) (p.blue * 4/25);
      }

      assign_sum_to_pixel(&current_pixel, _sum);
      dst[RIDX(i, j, dim)] = current_pixel;
    }
  }

}


/*
 * glow - Your current working version of glow.
 * IMPORTANT: This is the version you will be graded on
 */
char glow_descr[] = "glow: Current working version";
void glow(pixel *src, pixel *dst)
{
  g2(src, dst);
}

/*********************************************************************
 * register_glow_functions - Register all of your different versions
 *     of the glow kernel with the driver by calling the
 *     add_glow_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_glow_functions() {
  add_glow_function(&glow, glow_descr);
  add_glow_function(&naive_glow, naive_glow_descr);
}
