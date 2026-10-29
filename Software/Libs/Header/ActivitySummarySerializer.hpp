/**
 ******************************************************************************
 * @file    ActivitySummarySerializer.hpp
 * @brief   Serializes/Deserializes FreshBreath activity summary data to/from JSON.
 ******************************************************************************
 */

#ifndef ACTIVITY_SUMMARY_SERIALIZER_HPP
#define ACTIVITY_SUMMARY_SERIALIZER_HPP

#include "SDK/Kernel/Kernel.hpp"
#include "ActivitySummary.hpp"

class ActivitySummarySerializer {
public:
    ActivitySummarySerializer(const SDK::Kernel& kernel, const char* pathToFile);
    ~ActivitySummarySerializer() = default;

    bool save(const ActivitySummary& summary);
    bool load(ActivitySummary& summary);

private:
    const SDK::Kernel& mKernel;
    const char* mPath;
};

#endif // ACTIVITY_SUMMARY_SERIALIZER_HPP
