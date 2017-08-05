#include <mix/device_controller.h>
#include <mix/io_device.h>

#include "internal/helpers.hpp"

#include <map>

#include <cassert>

using namespace mix;

namespace {

struct DeviceProxy :
	public IIODevice
{
	DeviceProxy(IIODeviceListener& listener,
		DeviceId id,
		std::unique_ptr<IIODevice> device)
			: listener_{listener}
			, device_{std::move(device)}
			, id_{id}
	{
	}

	virtual bool ready() const override
	{
		return device_->ready();
	}

	virtual Word read()
	{
		listener_.on_device_read(id_);
		return device_->read();
	}

	virtual void write(const Word& word) override
	{
		listener_.on_device_write(id_);
		device_->write(word);
	}

private:
	IIODeviceListener& listener_;
	std::unique_ptr<IIODevice> device_;
	DeviceId id_;
};

} // namespace

struct DeviceController::Impl :
	public IIODeviceListener
{
	IIODeviceListener* listener_;
	std::map<DeviceId, DeviceProxy> devices_;

	Impl(IIODeviceListener* listener)
		: listener_{listener}
		, devices_{}
	{
	}

	IIODevice& device(DeviceId id)
	{
		return devices_.at(id);
	}

	void inject_device(DeviceId id, std::unique_ptr<IIODevice> device)
	{
		devices_.erase(id);
		(void)devices_.emplace(std::piecewise_construct,
			std::make_tuple(id),
			std::forward_as_tuple(*this, id, std::move(device)));
	}

	virtual void on_device_read(DeviceId id) override
	{
		internal::InvokeListener(listener_, &IIODeviceListener::on_device_read, id);
	}

	virtual void on_device_write(DeviceId id) override
	{
		internal::InvokeListener(listener_, &IIODeviceListener::on_device_write, id);
	}
};

DeviceController::DeviceController(IIODeviceListener* listener /*= nullptr*/)
	: impl_{std::make_unique<Impl>(listener)}
{
}

DeviceController::~DeviceController() = default;

void DeviceController::set_listener(IIODeviceListener* listener)
{
	impl_->listener_ = listener;
}

IIODevice& DeviceController::device(DeviceId id)
{
	return impl_->device(id);
}

void DeviceController::inject_device(DeviceId id, std::unique_ptr<IIODevice> device)
{
	assert(device);
	impl_->inject_device(id, std::move(device));
}