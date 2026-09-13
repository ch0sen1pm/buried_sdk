#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace buried {

class BuriedDbImpl;

class BuriedDb {
public:
    struct Data {
        std::int32_t id;
        std::int32_t priority;
        std::uint64_t timestamp;
        std::vector<char> content;
    };

    explicit BuriedDb(std::string db_path);

    ~BuriedDb();

    void InsertData(const Data& data);

    void DeleteData(const Data& data);

    void DeleteDatas(const std::vector<Data>& datas);

    std::vector<Data> QueryData(std::int32_t limit);

private:
    std::unique_ptr<BuriedDbImpl> impl_;
};


} // namespace buried