#pragma once

#ifndef PRODUCTIONPLANCALCULATE_H_
#define PRODUCTIONPLANCALCULATE_H_

#define PLAN_CALCULATE_API __declspec(dllexport)

#pragma comment(lib,"BaseForProTookit.lib")
#include <iostream>
#include <map>
#include <vector>

#include <afx.h>
#include "DynaArray.h"
#include "Logger.h"

class PLAN_CALCULATE_API stuPlanRet//生产计划排列结果
{
public:
	stuPlanRet() :nMaxDayLen(0), nDayCount(0), nWorkTotalDay(0), nMertTotalDay(0) {}

	stuPlanRet(stuPlanRet&& rhs)  noexcept :
		nMaxDayLen(std::exchange(rhs.nMaxDayLen, 0)),
		nDayCount(std::exchange(rhs.nDayCount, 0)),
		nWorkTotalDay(std::exchange(rhs.nWorkTotalDay, 0)),
		nMertTotalDay(std::exchange(rhs.nMertTotalDay, 0)),
		arrResult(std::move(rhs.arrResult)),
		arrWorkUsed(std::move(rhs.arrWorkUsed)) {}

	stuPlanRet &operator = (stuPlanRet&& rhs)  noexcept
	{
		nMaxDayLen = std::exchange(rhs.nMaxDayLen, 0);
		nDayCount = std::exchange(rhs.nDayCount, 0);
		nWorkTotalDay = std::exchange(rhs.nWorkTotalDay, 0);
		nMertTotalDay = std::exchange(rhs.nMertTotalDay, 0);
		arrResult = std::move(rhs.arrResult);
		arrWorkUsed = std::move(rhs.arrWorkUsed);
		return *this;
	}

	stuPlanRet(const stuPlanRet& rhs)
	{
		*this = rhs;
	};

	stuPlanRet &operator = (const stuPlanRet& rhs)
	{
		nMaxDayLen = rhs.nMaxDayLen;
		nDayCount = rhs.nDayCount;
		nWorkTotalDay = rhs.nWorkTotalDay;
		nMertTotalDay = rhs.nMertTotalDay;
		arrResult = rhs.arrResult;
		arrWorkUsed = rhs.arrWorkUsed;
		return *this;
	}
public:
	uint16_t nMaxDayLen;//生产周期
	uint16_t nDayCount;//产品累计生产时间
	uint16_t nWorkTotalDay;//工位使用累计天数
	uint16_t nMertTotalDay;//物料累计使用天数
	DynaArray2D<uint16_t> arrResult;//排列结果
	DynaArray2D<float> arrWorkUsed;//工位使用情况
};

stuPlanRet PLAN_CALCULATE_API GenerateProductionPlan(
	const uint16_t nTotalNum	//总排列数目
	, const uint16_t nPerNum	//每次排列数目
	, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
	, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
	, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
	, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
	, const std::vector<CString>& vecMertID	// material id
	, const std::vector<uint16_t>& vecMertNumPer);	 // material num per product

#endif
