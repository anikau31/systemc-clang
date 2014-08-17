//===========================================================================
//
// File Name    : tb_disparity.cpp
// Description  : Testbench
// Release Date : 16/07/13
// Author       : 
//
// Revision History
//---------------------------------------------------------------------------
// Date      Version   Author       Descriptione
//---------------------------------------------------------------------------
//16/07/13        1.0                 Send/receive and compare outputs 
//===========================================================================
#include "tb_disparity.h"

//
// Read input image
//

void test_disparity::initialize(){

  
  frame=0;
  x=0;
  y=0;

  // Open bmp file with stereoscopic image 
  in_file = fopen(INFILENAME,"rb");

  if (in_file){
    fread(&BmpHeader1, 18, sizeof(unsigned char), in_file);  
    fread(&width,        4, sizeof(unsigned char), in_file); 
    fread(&height,       4, sizeof(unsigned char), in_file); 
    fread(&BmpHeader2, 28, sizeof(unsigned char), in_file); 
    plusbytes = (4 - (width * 3) % 4) % 4;
    width  = width>>1;
  }else{
    cout<<"Could not read bmp file "<<  INFILENAME <<endl;
    sc_stop();
  }

  // Allocate memory for the image
  inbmp       = new unsigned char *[height];
  inbmpLeft   = new unsigned char *[height];
  inbmpRight  = new unsigned char *[height];

  for(y=0;y<height;y++)
    {

      inbmpLeft[y]  = new unsigned char [width*3];
      inbmpRight[y] = new unsigned char [width*3];
      inbmp[y]      = new unsigned char [width*6+plusbytes];
    }
  outbmp = new unsigned char *[HEIGHT];
  for(y=0;y<HEIGHT;y++){
    outbmp[y]    = new unsigned char [WIDTH];
    for(x=0;x<WIDTH;x++){
      outbmp[y][x]= 0;
    }
  }
  // Load image into memory
  for(y=height-1;y>=0;y--){
    fread(inbmp[y], 1, width*6+plusbytes, in_file);
    
    for(x=0;x<width;x++){
      inbmpLeft[y][x*3+2]=inbmp[y][x*3+2];
      inbmpLeft[y][x*3+1]=inbmp[y][x*3+1];
      inbmpLeft[y][x*3+0]=inbmp[y][x*3+0];
      
      inbmpRight[y][x*3+2]=inbmp[y][(width+x)*3+2];
      inbmpRight[y][x*3+1]=inbmp[y][(width+x)*3+1];
      inbmpRight[y][x*3+0]=inbmp[y][(width+x)*3+0];
    }
  }


  fclose(in_file);
}



//-------------------------
// Send input stimuli
//------------------------
void test_disparity::send(){

  // Variables declarations
  int xlimit=0;
  int ylimit=0;

  //Reset
  Right_pixel_R.write(0);
  Right_pixel_G.write(0);
  Right_pixel_B.write(0);
  Left_pixel_R.write(0);
  Left_pixel_G.write(0);
  Left_pixel_B.write(0);

  VSynch.write(false);
  HSynch.write(false);
  Height.write(0);
  Width.write(0);

  int finish_read = 0;
  valid_in.write(false);


  initialize();

  // -- Main loop for sending data to disparity estimator
  wait();


  while(true){

    Height.write(height);
    Width.write(width);
      
     for(y=0;y<(height+HOFFSET);y++){
      
       if((y<5)||(y>height+5)){
	 if(y<(HOFFSET/4)){
	   VSynch.write(true);
	 }
	 for(x=0; x < (width+WOFFSET); x++){
	   if(x<50)
	     HSynch.write(true);
	   else
	     HSynch.write(false);
	   wait();
	 }
       }
       else{
	 VSynch.write(false);
     
       
	 for(x=0;x<(width+WOFFSET);x++){
	   // Ignore edges
	   if(x<30  ){
	     HSynch.write(true);
	     valid_in.write(false);
	     Left_pixel_R.write(0);
	     Left_pixel_G.write(0);
	     Left_pixel_B.write(0);
	     Right_pixel_R.write(0);
	     Right_pixel_G.write(0);
	     Right_pixel_B.write(0);
	   }

	   else if((x<100)||(x>=(width+(WOFFSET/2)))){        
	     HSynch.write(false);
	     valid_in.write(false);
	     Left_pixel_R.write(0);
	     Left_pixel_G.write(0);
	     Left_pixel_B.write(0);
	     Right_pixel_R.write(0);
	     Right_pixel_G.write(0);
	     Right_pixel_B.write(0);
	   } 


	   else{
	     // Set control signals to start writting data
	     HSynch.write(false);
	     valid_in.write(true);
	     
	     if(y-(HOFFSET/4) >= height)
	       ylimit = height-1;
	     else
	       ylimit = y - (HOFFSET/4);
	    
	     if(x - (WOFFSET/2)>= width )
	       xlimit = width -1;
	       else
		 xlimit = x- (WOFFSET/2);

	     // Left image RGB
	     Left_pixel_R.write(inbmpLeft[ylimit][xlimit*3+2]);
	     Left_pixel_G.write(inbmpLeft[ylimit][xlimit*3+1]);
	     Left_pixel_B.write(inbmpLeft[ylimit][xlimit*3+0]);

	     // Right image RGB
	     Right_pixel_R.write(inbmpRight[ylimit][xlimit*3+2]);
	     Right_pixel_G.write(inbmpRight[ylimit][xlimit*3+1]);
	     Right_pixel_B.write(inbmpRight[ylimit][xlimit*3+0]);
	   }
            

	   
	   wait();
	        }//x for_loop
       }     //else
     }   //y for_loop


     compare_results();
     sc_stop();

  }//while_loop
}

//-----------------------------------------
// Receive bmp from the disparity estimator
//----------------------------------------
void test_disparity::recv(){

  // Variables declaration
  int height = Height.read();
  int hsize_temp;
  int hd_temp;  
  int hbnd_temp;

  // Initialization
  if(height==0){
    hsize_temp = 0;
    hd_temp    = 0;   
    hbnd_temp  = 0;
  }
  else{
    if(height%HEIGHT == 0)
      hsize_temp = height/HEIGHT;
    else
      hsize_temp = (height/HEIGHT)+1;
    

    hd_temp    = height/hsize_temp;     
    hbnd_temp  = ((height - hd_temp*hsize_temp)/2) + (((height - hd_temp*hsize_temp)/2)%2);
  }


  VSynch_single = false;
  HSynch_single = false;

  yi       = 0;
  out_y    = 0;
  out_x    = 0;

  wait();

  // main receiving data loop
  while(true){
    height = Height.read();

    if(height==0){
      hsize_temp = 0;
      hd_temp    = 0;     
      hbnd_temp  = 0;
    }
    else{
      if(height%HEIGHT == 0)
	hsize_temp =height/HEIGHT;
      else
	hsize_temp = (height/HEIGHT)+1;

      hd_temp    = height/hsize_temp;    
      hbnd_temp  = ((height - hd_temp*hsize_temp)/2) + (((height - hd_temp*hsize_temp)/2)%2);
    }

    if((VSynch_single==true) && (VSynch.read()==false)){
      yi=0;
      out_y=0;
      out_x=0;

    }
    else if((HSynch_single==true) && (HSynch.read()==false)){
      yi=yi+1;
      out_y= (yi-hbnd_temp)/hsize_temp;
      out_x=0;
    }
    else if(valid_out==true){
      out_y= ((yi-2-hbnd_temp)/hsize_temp);
      outbmp[out_y][out_x]= depth_out.read();
      out_x++;
    }
    else{
      yi=yi;
      out_y=out_y;
      out_x=out_x;
    }
    
    VSynch_single = VSynch.read();
    HSynch_single = HSynch.read();

    wait();
  }
}


//----------------------------
// Compare results function
//----------------------------
void test_disparity::compare_results(){
    
  // Variables
  int errors=0;

  // Create bmp file with results
  out_file     = fopen(OUTFILENAME,"wb");
  if (out_file){
    width  = WIDTH;
    height = HEIGHT;
    
    fwrite(&BmpHeader1, 18, sizeof(unsigned char), out_file);
    fwrite(&width,        4, sizeof(unsigned char), out_file);
    fwrite(&height,       4, sizeof(unsigned char), out_file);
    fwrite(&BmpHeader2, 28, sizeof(unsigned char), out_file);
  }
  else{
    cout<<"Could not open bmp file " << OUTFILENAME << endl;
    sc_stop();
    exit (-1);
  }
  for(y=HEIGHT-1;y>=0;y--){
    for(x=0;x<WIDTH;x++){
      fwrite(&outbmp[y][x], 1, 1, out_file);
      fwrite(&outbmp[y][x], 1, 1, out_file);
      fwrite(&outbmp[y][x], 1, 1, out_file);
      if(x==(WIDTH-1)&&(plusbytes!=0))
	fwrite(0, 1, plusbytes, out_file);
    }
  }

    //------------------------
    //Load the golden output
    //------------------------
    compare_file = fopen(OUTFILENAME_GOLDEN,"rb");
    if (compare_file){
      fread(&BmpHeader1, 18, sizeof(unsigned char), compare_file);     
      fread(&width,        4, sizeof(unsigned char), compare_file);  
      fread(&height,       4, sizeof(unsigned char), compare_file);      
      fread(&BmpHeader2, 28, sizeof(unsigned char), compare_file);      
      plusbytes = (4 - (width * 3) % 4) % 4;
      if((height!=HEIGHT)||(width!=WIDTH)){
        cout<<"The size of compare bmp is error"<<endl;
        sc_stop();
	exit (-1);
      }

    }else{
      cout<<"Coult not open file " << OUTFILENAME_GOLDEN << endl;
      sc_stop();
      exit (-1);
    }
    
    comparebmp  = new unsigned char *[HEIGHT];
    for(y=0;y<HEIGHT;y++)
      comparebmp[y]    = new unsigned char [WIDTH*3];

    for(y=HEIGHT-1;y>=0;y--)
      fread(comparebmp[y], 1, WIDTH*3+plusbytes, compare_file);


    
    // Write comparison result
   diff_file    = fopen(DIFFFILENAME,"wb");
    for(y=0;y<HEIGHT;y++)
      for(x=0;x<WIDTH;x++)
        if((int)outbmp[y][x]!=(int)comparebmp[y][x*3]){
          cout<<"Output missmatch (y,x)=("<<y<<","<<x<<"): output= "<<(int)outbmp[y][x]<<" , golden= "<<(int)comparebmp[y][x*3]<<endl;
          fprintf(diff_file,"Output missmatch (y,x)=(%d,%d): output= %d , golden= %d;\n",y,x,outbmp[y][x],comparebmp[y][x*3]);
	  errors ++;
        }


   if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;

    fclose(out_file);
    fclose(compare_file);
    fclose(diff_file);

}

