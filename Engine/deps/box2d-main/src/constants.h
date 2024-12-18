// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

extern float b2_lengthUnitsPerMeter;

// Used to detect bad values. Positions greater than about 16km will have precision
// problems, so 100km as a limit should be fine in all cases.
#define b2_huge ( 100000.0f * b2_lengthUnitsPerMeter )

// Maximum parallel workers. Used to size some static arrays.
#define b2_maxWorkers 64

// Maximum number of colors in the constraint graph. Constraints that cannot
// find a color are added to the overflow set which are solved single-threaded.
#define b2_graphColorCount 12

// A small length used as a collision and constraint tolerance. Usually it is
// chosen to be numerically significant, but visually insignificant. In meters.
// @warning modifying this can have a significant impact on stability
#define b2_linearSlop ( 0.005f * b2_lengthUnitsPerMeter )

// Maximum number of simultaneous worlds that can be allocated
#define b2_maxWorlds 128

// The maximum rotation of a body per time step. This limit is very large and is used
// to prevent numerical problems. You shouldn't need to adjust this.
// @warning increasing this to 0.5f * b2_pi or greater will break continuous collision.
#define b2_maxRotation ( 0.25f * b2_pi )

// @warning modifying this can have a significant impact on performance and stability
#define b2_speculativeDistance ( 4.0f * b2_linearSlop )

// This is used to fatten AABBs in the dynamic tree. This allows proxies
// to move by a small amount without triggering a tree adjustment. This is in meters.
// @warning modifying this can have a significant impact on performance
#define b2_aabbMargin ( 0.05f * b2_lengthUnitsPerMeter )

// The time that a body must be still before it will go to sleep. In seconds.
#define b2_timeToSleep 0.5f

enum b2TreeNodeFlags
{
	b2_allocatedNode = 0x0001,
	b2_enlargedNode = 0x0002,
	b2_leafNode = 0x0004,
};
