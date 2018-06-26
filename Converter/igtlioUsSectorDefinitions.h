#ifndef IGTLIOUSSECTORDEFINITIONS_H
#define IGTLIOUSSECTORDEFINITIONS_H


// This defines the meta information tag names for OpenIGTLinkIO ultrasound messages
#define IGTLIO_KEY_PROBE_TYPE   "ProbeType"
#define IGTLIO_KEY_ORIGIN       "Origin"
#define IGTLIO_KEY_ANGLES       "Angles"
#define IGTLIO_KEY_BOUNDING_BOX "BoundingBox"
#define IGTLIO_KEY_DEPTHS       "Depths"
#define IGTLIO_KEY_LINEAR_WIDTH "LinearWidth"
#define IGTLIO_KEY_SPACING_X    "SpacingX"
#define IGTLIO_KEY_SPACING_Y    "SpacingY"

enum IGTLIO_PROBE_TYPE
{
  UNKNOWN,
  SECTOR,
  LINEAR,
  MECHANICAL
};

#endif // IGTLIOUSSECTORDEFINITIONS_H
