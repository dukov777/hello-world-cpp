#include <iostream>
#include "hello.h"
#include <vector>

// Interface for the Capacitor class

class CapacitorInterface
{
public:
	virtual void calculate(float frequency, float current) = 0;
	virtual ~CapacitorInterface() {}
	virtual std::string getName() = 0;
	virtual float getCurrent() = 0;
	virtual float getVoltage() = 0;
	virtual float getPower() = 0;
	virtual float getCapacitance() = 0;
	virtual float getXc() = 0;
};;

// implement base class for the CapacitorInterface
class CapacitorBase : public CapacitorInterface
{
protected:
	/* data */
	std::string name;
	float capacitance;
	float max_voltage;
	float max_current;
	float max_power;

	float xc;
	float current;
	float voltage;
	float power;
public:
	std::string getName() { return name; }
	float getCurrent() { return current; }
	float getVoltage() { return voltage; }
	float getPower() { return power; }
	float getCapacitance() { return capacitance; }
	float getXc() { return xc; }

	CapacitorBase(const std::string& name, float capacitance, float max_voltage, float max_current, float max_power);
	~CapacitorBase();
};

CapacitorBase::CapacitorBase(const std::string& name, float capacitance, float max_voltage, float max_current, float max_power)
{
	this->name = name;
	this->capacitance = capacitance;
	this->max_voltage = max_voltage;
	this->max_current = max_current;
	this->max_power = max_power;
}

CapacitorBase::~CapacitorBase()
{
}

class Capacitor : public CapacitorBase
{
private:
	/* data */

public:
	Capacitor(const std::string& name, float capacitance, float max_voltage, float max_current, float max_power);
	~Capacitor();
	void calculate(float frequency, float current);

};

void Capacitor::calculate(float frequency, float current){
	xc = 1 / (2 * 3.14159 * frequency * capacitance);
	voltage = xc * current;
	power = voltage * current;
	this->current = current;
}

Capacitor::Capacitor(const std::string& name, float capacitance, float max_voltage, float max_current, float max_power) 
	: CapacitorBase(name, capacitance, max_voltage, max_current, max_power)	// call the base class constructor
{
}

Capacitor::~Capacitor()
{
}

// Serial Capacitors Circuit Tank
class SerialCapacitors : public CapacitorBase
{
private:
	/* data */
	std::vector<CapacitorInterface*> caps;

public:
	SerialCapacitors(std::string name, std::vector<CapacitorInterface*>& caps);
	~SerialCapacitors();
	void calculate(float frequency, float current);
};

SerialCapacitors::SerialCapacitors(std::string name, std::vector<CapacitorInterface*>& caps) 
	: CapacitorBase(name, 0, 0, 0, 0)	// call the base class constructor
{
	this->caps = caps;
}

SerialCapacitors::~SerialCapacitors()
{
}

void SerialCapacitors::calculate(float frequency, float current){
	float total_voltage = 0;
	float total_power = 0;
	float total_xc = 0;

	for (int i = 0; i < caps.size(); i++){
		caps[i]->calculate(frequency, current);
		total_voltage += caps[i]->getVoltage();
		total_power += caps[i]->getPower();
		total_xc += caps[i]->getXc();
	}

	this->voltage = total_voltage;
	this->power = total_power;
	this->xc = total_xc;
	this->current = current;
}



class CurrentMonitorDecorator : public CapacitorInterface
{
private:
	/* data */
	CapacitorInterface* cap;
public:
	CurrentMonitorDecorator(CapacitorInterface* cap);
	~CurrentMonitorDecorator();
	void calculate(float frequency, float current);
	std::string getName() { return cap->getName(); }
	float getCurrent() { return cap->getCurrent(); }
	float getVoltage() { return cap->getVoltage(); }
	float getPower() { return cap->getPower(); }
	float getCapacitance() { return cap->getCapacitance(); }
	float getXc() { return cap->getXc(); }

};

CurrentMonitorDecorator::CurrentMonitorDecorator(CapacitorInterface* cap)
{
	this->cap = cap;
}


CurrentMonitorDecorator::~CurrentMonitorDecorator()
{
}

void CurrentMonitorDecorator::calculate(float frequency, float current){
	cap->calculate(frequency, current);
	std::cout << "Capacitor: " << cap->getName()
		<< ", Current: " << cap->getCurrent() 
		<< ", Voltage: " << cap->getVoltage() 
		<< ", Power: " << cap->getPower()		
		<< "\n";
}

void hello(){
	std::cout << "Hello World!\n";
	// CapacitorInterface* cap = new CurrentMonitorDecorator(
	// 	new Capacitor(1e-6, 1000, 100, 500e3));
	// cap->calculate(60, 10);

	std::vector<CapacitorInterface*> caps =
		{new CurrentMonitorDecorator(new Capacitor("C1", 1e-6, 1000, 100, 500e3)),
		 new CurrentMonitorDecorator(new Capacitor("C2", 2e-6, 1000, 100, 500e3))};

	std::vector<CapacitorInterface*> caps2 =
		{new CurrentMonitorDecorator(new SerialCapacitors("Serial 1", caps)),
		 new CurrentMonitorDecorator(new SerialCapacitors("Serial 2", caps))};

	auto serial_tank = new SerialCapacitors("Tank Circuit Example", caps2);
	// auto serial_tank = new SerialCapacitors(caps);
	serial_tank->calculate(60, 1);
	std::cout << "Total Voltage: " << serial_tank->getVoltage() << "\n";
	std::cout << "Total Power: " << serial_tank->getPower() << "\n";
	std::cout << "Total Xc: " << serial_tank->getXc() << "\n";
	std::cout << "Total Current: " << serial_tank->getCurrent() << "\n";

}