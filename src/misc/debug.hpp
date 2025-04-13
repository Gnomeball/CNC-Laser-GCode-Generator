#ifndef debug_h
#define debug_h

/* If we should print out the config values

Current Config :
  Laser Power   = 75%
  Travel Speed  = 500
  Has Outline   = true
  Outline Speed = 60
  Has Infill    = true
  Infill Speed  = 60
  Density       = 5

Note: The above values are the intended defaults

*/
#define DEBUG_CONFIG_PRINT_OUT

/* If we should print out grid creation stages;

Creating Grids
  Grid padding set : H W
  Master Grid Created
  Master Grid De-Artefacted : P pixels removed
  Outline Grid Created
  Infill Grid Created

*/
#define DEBUG_GRIDS

/* If we should print grids out to file

Writing Grids to File

*/
#define DEBUG_GRID_PRINT_OUT

/* If we should print out outline creation stages;

Constructing Outline :
  Found Outline Points : P points
  Generated Outline Parts : C + D lines
  Removed Unnecessary Lines : R lines removed
  Found Outline : L lines
  Ordered Outline
  Cleaned Outline
  Outline Continuation Configured
  Outline Stats Calculated
  Outline Construction Complete

*/
#define DEBUG_OUTLINE

/* If we should print out infill creation stages;

Constructing Infill :
  Found Infill : L lines
  Starting Infill at : (x, y)
  Ordered Infill
  Infill Stats Calculated
  Infill Construction Complete

*/
#define DEBUG_INFILL

#endif // debug_h
