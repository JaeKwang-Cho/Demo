// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class DEMO_API AEnemyPanchi;

#include <memory>

class Visitor
{
public:
	virtual void Visit(AEnemyPanchi* Panchi){}
	//Add Visit 
};

class DescVisitor : public Visitor
{
public:
	virtual void Visit(AEnemyPanchi* Panchi) override;
};

class AttackVisitor : public Visitor
{
public:
	virtual void Visit(AEnemyPanchi* Panchi) override;
};

using VisitorPtr = std::shared_ptr<Visitor>;