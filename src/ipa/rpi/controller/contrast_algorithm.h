/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2019, Raspberry Pi Ltd
 *
 * contrast_algorithm.h - contrast (gamma) control algorithm interface
 */
#pragma once

#include "algorithm.h"

namespace RPiController {

class ContrastAlgorithm : public Algorithm
{
public:
	ContrastAlgorithm(Controller *controller) : Algorithm(controller) {}
	/* A contrast algorithm must provide the following: */
	virtual void setBrightness(double brightness) = 0;
	virtual void setContrast(double contrast) = 0;
	virtual void setConfigLoHistogram(double loHistogram) = 0;
	virtual void setConfigLoLevel(double loLevel) = 0;
	virtual void setConfigLoMax(double loMax) = 0;
	virtual void setConfigHiHistogram(double hiHistogram) = 0;
	virtual void setConfigHiLevel(double hiLevel) = 0;
	virtual void setConfigHiMax(double hiMax) = 0;
};

} /* namespace RPiController */
