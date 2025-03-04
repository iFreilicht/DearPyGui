#include "mvApp.h"
#include "mvModule_Core.h"
#include "mvViewport.h"
#include "mvCallbackRegistry.h"
#include "mvInput.h"
#include <thread>
#include <future>
#include <chrono>
#include "mvProfiler.h"
#include <implot.h>
#include "mvEventListener.h"
#include "mvItemRegistry.h"
#include "mvFontManager.h"
#include "mvThemeManager.h"
#include "mvCallbackRegistry.h"
#include "mvPythonTranslator.h"
#include "mvPythonExceptions.h"
#include "mvGlobalIntepreterLock.h"
#include <frameobject.h>
#include "mvModule_Core.h"
#include "mvLog.h"
#include "mvEventMacros.h"
#include "mvToolManager.h"

namespace Marvel {

	mvApp* mvApp::s_instance = nullptr;
	std::atomic_bool mvApp::s_started = false;
	std::atomic_bool mvApp::s_manualMutexControl = false;
	float mvApp::s_deltaTime = 0.0f;
	double mvApp::s_time = 0.0;
	std::mutex mvApp::s_mutex = {};

	mvApp* mvApp::GetApp()
	{
		mvLog::Init();

		if (s_instance)
			return s_instance;

		s_instance = new mvApp();
		return s_instance;
	}

	void mvApp::DeleteApp()
	{
		if (s_instance)
		{
			delete s_instance;
			s_instance = nullptr;
		}

		s_started = false;
	}

	void mvApp::SetAppStopped() 
	{ 

        GetApp()->getCallbackRegistry().stop();
        GetApp()->getCallbackRegistry().addCallback(nullptr, "null", nullptr, nullptr);
        GetApp()->m_future.get();
		s_started = false; 
		auto viewport = s_instance->getViewport();
		if (viewport)
			viewport->stop();
	}

	void mvApp::cleanup()
	{
		getCallbackRegistry().stop();

		getCallbackRegistry().addCallback(nullptr, "null", nullptr, nullptr);
      
		m_future.get();
		delete m_viewport;
		s_started = false;
	}

	mvApp::mvApp()
	{

		m_mainThreadID = std::this_thread::get_id();

#if defined(MV_DEBUG)
		new mvEventListener();
#endif // MV_PROFILE

		// create managers
		m_itemRegistry = CreateOwnedPtr<mvItemRegistry>();
		m_themeManager = CreateOwnedPtr<mvThemeManager>();
        m_callbackRegistry = CreateOwnedPtr<mvCallbackRegistry>();

	}

    mvCallbackRegistry& mvApp::getCallbackRegistry()
    { 
        return *m_callbackRegistry; 
    }

	mvItemRegistry& mvApp::getItemRegistry() 
	{ 
		return *m_itemRegistry; 
	}

	mvThemeManager& mvApp::getThemeManager()
	{
		return *m_themeManager;
	}

	mvApp::~mvApp()
	{
		m_itemRegistry->clearRegistry();

		mvLog::Cleanup();
	
	}

	void mvApp::turnOnDocking(bool dockSpace)
	{ 
		m_docking = true; 
		m_dockingViewport = dockSpace;
	}

	void mvApp::render()
	{

		// update timing
		s_deltaTime = ImGui::GetIO().DeltaTime;
		s_time = ImGui::GetTime();
		ImGui::GetIO().FontGlobalScale = mvToolManager::GetFontManager().getGlobalFontScale();

		if (m_dockingViewport)
			ImGui::DockSpaceOverViewport();


		mvEventBus::Publish(mvEVT_CATEGORY_APP, mvEVT_FRAME, {CreateEventArgument("FRAME", ImGui::GetFrameCount() )});

		// route input callbacks
		mvInput::CheckInputs();

		mvToolManager::Draw();

        std::lock_guard<std::mutex> lk(s_mutex);
		mvEventBus::Publish(mvEVT_CATEGORY_APP, mvEVT_PRE_RENDER);
		mvEventBus::Publish(mvEVT_CATEGORY_APP, mvEVT_PRE_RENDER_RESET);
		mvEventBus::Publish(mvEVT_CATEGORY_APP, mvEVT_RENDER);
		mvEventBus::Publish(mvEVT_CATEGORY_APP, mvEVT_END_FRAME);
	}

	bool mvApp::checkIfMainThread() const
	{
		return std::this_thread::get_id() == m_mainThreadID;
	}

	std::map<std::string, mvPythonParser>& mvApp::getParsers()
	{ 
		return const_cast<std::map<std::string, mvPythonParser>&>(mvModule_Core::GetModuleParsers()); 
	}

	void mvApp::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		{
			mvPythonParser parser(mvPyDataType::None);
			parser.addArg<mvPyDataType::Bool>("dock_space", mvArgType::KEYWORD_ARG, "False", "add explicit dockspace over viewport");
			parser.finalize();
			parsers->insert({ "enable_docking", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::Bool);
			parser.finalize();
			parsers->insert({ "is_dearpygui_running", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::None);
			parser.addArg<mvPyDataType::String>("viewport", mvArgType::KEYWORD_ARG, "''");
			parser.finalize();
			parsers->insert({ "setup_dearpygui", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::None);
			parser.finalize();
			parsers->insert({ "render_dearpygui_frame", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::None);
			parser.finalize();
			parsers->insert({ "cleanup_dearpygui", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::None);
			parser.finalize();
			parsers->insert({ "stop_dearpygui", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::Float);
			parser.finalize();
			parsers->insert({ "get_total_time", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::Float);
			parser.finalize();
			parsers->insert({ "get_delta_time", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::String);
			parser.finalize();
			parsers->insert({ "get_dearpygui_version", parser });
		}

	}

	PyObject* mvApp::lock_mutex(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		mvApp::s_mutex.lock();
		mvApp::s_manualMutexControl = true;

		return GetPyNone();
	}

	PyObject* mvApp::unlock_mutex(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		mvApp::s_mutex.unlock();
		mvApp::s_manualMutexControl = false;

		return GetPyNone();
	}

	PyObject* mvApp::enable_docking(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		int dockspace = false;

		if (!(mvApp::GetApp()->getParsers())["enable_docking"].parse(args, kwargs, __FUNCTION__,
			&dockspace))
			return GetPyNone();

		mvApp::GetApp()->getCallbackRegistry().submit([=]()
			{
				mvApp::GetApp()->turnOnDocking(dockspace);
			});

		return GetPyNone();
	}

	PyObject* mvApp::is_dearpygui_running(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		return ToPyBool(mvApp::IsAppStarted());
	}

	PyObject* mvApp::setup_dearpygui(PyObject* self, PyObject* args, PyObject* kwargs)
	{

		const char* viewport = "";

		if (!(mvApp::GetApp()->getParsers())["setup_dearpygui"].parse(args, kwargs, __FUNCTION__,
			&viewport))
			return GetPyNone();

		Py_BEGIN_ALLOW_THREADS;
		mvLog::Init();

		if (mvApp::IsAppStarted())
		{
			mvThrowPythonError(1000, "Cannot call \"setup_dearpygui\" while a Dear PyGUI app is already running.");
			return GetPyNone();
		}

		GetApp()->getItemRegistry().emptyParents();
		s_started = true;
		GetApp()->m_future = std::async(std::launch::async, []() {return GetApp()->m_callbackRegistry->runCallbacks(); });

		MV_CORE_INFO("application starting");

		Py_END_ALLOW_THREADS;

		return GetPyNone();
	}

	PyObject* mvApp::render_dearpygui_frame(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		MV_PROFILE_SCOPE("Frame")

		Py_BEGIN_ALLOW_THREADS;
		auto window = mvApp::GetApp()->getViewport();
		window->renderFrame();
		Py_END_ALLOW_THREADS;

		return GetPyNone();
	}

	PyObject* mvApp::cleanup_dearpygui(PyObject* self, PyObject* args, PyObject* kwargs)
	{

		Py_BEGIN_ALLOW_THREADS;
		mvApp::GetApp()->cleanup();	
		mvApp::DeleteApp();
		mvEventBus::Reset();
		Py_END_ALLOW_THREADS;

		return GetPyNone();
	}

	PyObject* mvApp::stop_dearpygui(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		mvApp::StopApp();
		auto viewport = mvApp::GetApp()->getViewport();
		if (viewport)
			viewport->stop();
		return GetPyNone();
	}

	PyObject* mvApp::get_total_time(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		if(!mvApp::s_manualMutexControl) std::lock_guard<std::mutex> lk(mvApp::s_mutex);
		return ToPyFloat((float)mvApp::s_time);
	}

	PyObject* mvApp::get_delta_time(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		if (!mvApp::s_manualMutexControl) std::lock_guard<std::mutex> lk(mvApp::s_mutex);
		return ToPyFloat(mvApp::s_deltaTime);

	}

	PyObject* mvApp::get_dearpygui_version(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		return ToPyString(mvApp::GetApp()->GetVersion());
	}

}
