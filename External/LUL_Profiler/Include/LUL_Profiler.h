#ifndef LUL_PROFILER_H
    #define LUL_PROFILER_H
    
    #include <process.h>

    #ifdef __cplusplus 
        #include <chrono>
        #include <vector>
        
        #if 1
            #define LUL_PROFILER_TIMER_SET_OUTPUT_DIR(path) LUL_::Profiler::Timer::Get().SetPath(path)
            #define LUL_PROFILER_TIMER_START() LUL_::Profiler::Snapshot ProfilerSnapshot(__FUNCSIG__, _getpid())
            #define LUL_PROFILER_TIMER_RESULTS() LUL_::Profiler::Timer::Get().OutputResults()
        #else
            #define LUL_PROFILER_TIMER_SET_OUTPUT_DIR(path) 
            #define LUL_PROFILER_TIMER_START() 
            #define LUL_PROFILER_TIMER_RESULTS()
        #endif // LUL_PROFILING

        #pragma warning(push)
        // - 4251  - Needs to have dll - interface to be used by clients of class.
        // - 26495 - Variable is uninitialized. Always initialize a member variable(type.6).
        #pragma warning(disable : 4251 26495)

        namespace LUL_::Profiler
        {
            struct SnapshotData
            {
                char const* const Name;
                int PID;
                unsigned long TID;
                std::chrono::time_point<std::chrono::high_resolution_clock> Start, Stop;
            };

            struct Snapshot
            {
            public:
                Snapshot() = delete;
                Snapshot(Snapshot&&) = default;
                Snapshot(const Snapshot&) = delete;

                Snapshot(char const* const fnsig, int pid) noexcept;

                ~Snapshot() noexcept;

            private:

                SnapshotData m_Data;
            };

            class Timer
            {
            private:

                friend Snapshot;

                std::vector<SnapshotData> m_Snapshots = std::vector<SnapshotData>();
                std::wstring m_OutputPath = L"";

            private:

                // Singleton  --------------------------------------------------

                Timer() = default;

            public:

                Timer(Timer&&) = delete;
                Timer(const Timer&) = delete;

                static Timer& Get() noexcept;

                ~Timer() noexcept = default;

            public:

                void OutputResults();

            public:

                // Setters ---------------------------------------------------------------------

                void SetPath(const wchar_t* path);

            private:

                void AddSnapshot(SnapshotData s);

            private:

                /*
                * @returns Path to directory with results */
                static const wchar_t* CreateResultDir();

                void WriteToJson(std::wofstream& fst,
                    const SnapshotData& s,
                    const long long& dur,
                    const long long& start);
                
                void WriteToFile(std::wstring path);
            };
        }

        #pragma warning(pop)
    #else // C -----------------------------------------------------------------
    #endif // __cplusplus
#endif // !LUL_PROFILER_H
