#include "database/database.h"

#include <memory>
#include <utility>

#include "sqlite_orm/sqlite_orm.h"

using namespace sqlite_orm;

namespace buried {

inline auto InitStorage(const std::string& path) {
    return make_storage(
        path,
        make_table(
            "buried_data",
            make_column("id", &BuriedDb::Data::id,
                        primary_key().autoincrement()),
            make_column("priority", &BuriedDb::Data::priority),
            make_column("timestamp", &BuriedDb::Data::timestamp),
            make_column("content", &BuriedDb::Data::content)));
}

class BuriedDbImpl {
public:
    using DBStorage = decltype(InitStorage(""));

    explicit BuriedDbImpl(std::string db_path)
        : db_path_(std::move(db_path)),
          storage_(std::make_unique<DBStorage>(InitStorage(db_path_))) {
        storage_->sync_schema();
    }

    ~BuriedDbImpl() = default;

    void InsertData(const BuriedDb::Data& data) {
        auto guard = storage_->transaction_guard();

        storage_->insert(data);

        guard.commit();
    }

    void DeleteData(const BuriedDb::Data& data) {
        auto guard = storage_->transaction_guard();

        storage_->remove_all<BuriedDb::Data>(
            where(c(&BuriedDb::Data::id) == data.id));

        guard.commit();
    }

    void DeleteDatas(const std::vector<BuriedDb::Data>& datas) {
        auto guard = storage_->transaction_guard();

        for (const auto& data: datas) {
            storage_->remove_all<BuriedDb::Data>(
                where(c(&BuriedDb::Data::id) == data.id));
        }

        guard.commit();
    }

    std::vector<BuriedDb::Data> QueryData(std::int32_t limit_size) {
        return storage_->get_all<BuriedDb::Data>(
            order_by(&BuriedDb::Data::priority).desc(),
            limit(limit_size));
    }

private:
    std::string db_path_;
    std::unique_ptr<DBStorage> storage_;
};

BuriedDb::BuriedDb(std::string db_path)
    : impl_(std::make_unique<BuriedDbImpl>(std::move(db_path))) {}

BuriedDb::~BuriedDb() = default;

void BuriedDb::InsertData(const Data& data) {
    impl_->InsertData(data);
}

void BuriedDb::DeleteData(const Data& data) {
    impl_->DeleteData(data);
}

void BuriedDb::DeleteDatas(const std::vector<Data>& datas) {
    impl_->DeleteDatas(datas);
}

std::vector<BuriedDb::Data> BuriedDb::QueryData(std::int32_t limit) {
    return impl_->QueryData(limit);
}

} // namespace buried