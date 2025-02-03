//
// Created by TheDaChicken on 7/23/2024.
//
#include <gtest/gtest.h>

#include <models/LinkedChannelModel.h>
#include <QSqlRecord>
#include <QMimeData>

class SqlTest : public testing::Test
{
 protected:
  void SetUp() override
  {
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(":memory:");
	db.open();

	model = std::make_unique<LinkedChannelModel>(nullptr, db);
	model->SetTable("favorites");
  }

  void SetupDefault()
  {
	ASSERT_EQ(model->AddChannel(one), true);
	ASSERT_EQ(model->AddChannel(two), true);
	ASSERT_EQ(model->AddChannel(three), true);
	ASSERT_EQ(model->AddChannel(four), true);

	{
	  SCOPED_TRACE(DescribeModel());

	  ASSERT_EQ(model->GetChannel(model->index(0, 0)), one);
	  ASSERT_EQ(model->GetChannel(model->index(1, 0)), two);
	  ASSERT_EQ(model->GetChannel(model->index(2, 0)), three);
	  ASSERT_EQ(model->GetChannel(model->index(3, 0)), four);
	}

	ASSERT_EQ(model->submit(), true);
  }

  void TearDown() override
  {
	QSqlDatabase::removeDatabase(db.connectionName());
	model.reset();
  }

  std::string DescribeModel()
  {
	std::string result;
	for (int i = 0; i < model->rowCount(); i++)
	{
	  RadioChannel channel = model->GetChannel(model->index(i, 0));
	  int prev = model->GetRecord(i).value("prev").toInt();

	  result += channel.GetDisplayChannel().toStdString() + " (" + std::to_string(prev) + "), ";
	}
	return result;
  }

  QSqlDatabase db;
  std::unique_ptr<LinkedChannelModel> model;

  RadioChannel one = {MOD_FM, 92.5, 0};
  RadioChannel two = {MOD_FM, 93.3, 0};
  RadioChannel three = {MOD_FM, 94.1, 0};
  RadioChannel four = {MOD_FM, 95.1, 0};
};

TEST_F(SqlTest, Simple)
{
  // Test adding and removing rows
  ASSERT_EQ(model->rowCount(), 0);
  ASSERT_EQ(model->insertRows(0, 1, QModelIndex()), true);
  ASSERT_EQ(model->rowCount(), 1);
  ASSERT_EQ(model->removeRows(0, 1, QModelIndex()), true);
  ASSERT_EQ(model->rowCount(), 0);

  ASSERT_EQ(model->submit(), true);

  // Test adding and removing channels
  ASSERT_EQ(model->AddChannel(one), true);
  ASSERT_EQ(model->rowCount(), 1);
  ASSERT_EQ(model->RemoveChannel(one), true);
  ASSERT_EQ(model->rowCount(), 0);

  // Test the movement of rows due to deletion
  ASSERT_EQ(model->AddChannel(two), true);
  ASSERT_EQ(model->AddChannel(one), true);
  ASSERT_EQ(model->rowCount(), 2);
  ASSERT_EQ(model->RemoveChannel(two), true);
  ASSERT_EQ(model->GetChannel(model->index(0, 0)), one);

  // Test the movement of rows due to insertion
  ASSERT_EQ(model->AddChannel(two), true);
  ASSERT_EQ(model->GetChannel(model->index(1, 0)), two);
  ASSERT_EQ(model->GetChannel(model->index(0, 0)), one);
}

/**
 * @brief Test the complex operations of the model
 */
TEST_F(SqlTest, MovingRows)
{
  SetupDefault();

  // Test moving rows
  // Moved at 0 to 2 (92.5, 93.3, 94.1, 95.1) -> (93.3, 94.1, 92.5, 95.1)
  ASSERT_EQ(model->moveRows(QModelIndex(), 0, 1, QModelIndex(), 2), true);
  ASSERT_EQ(model->submit(), true);

  {
	SCOPED_TRACE(DescribeModel());

	ASSERT_EQ(model->GetChannel(model->index(0, 0)), two);
	ASSERT_EQ(model->GetChannel(model->index(1, 0)), three);
	ASSERT_EQ(model->GetChannel(model->index(2, 0)), one);
	ASSERT_EQ(model->GetChannel(model->index(3, 0)), four);
  }
}

TEST_F(SqlTest, Drop)
{
  SetupDefault();

  // Moved at 0 to 1
  QMimeData data;
  // Unused freq
  RadioChannel temp_channel = {MOD_FM, 95.5, 0};

  data.setData("application/json",
			   QByteArray::fromStdString(nlohmann::json::array({temp_channel}).dump()));

  ASSERT_EQ(model->dropMimeData(&data, Qt::MoveAction, 1, 0, QModelIndex()), true);
  ASSERT_EQ(model->submit(), true);

  {
	SCOPED_TRACE(DescribeModel());

	ASSERT_EQ(model->GetChannel(model->index(0, 0)), one);
	ASSERT_EQ(nlohmann::json(model->GetChannel(model->index(1, 0))).dump(),
			  nlohmann::json(temp_channel).dump());
	ASSERT_EQ(model->GetChannel(model->index(1, 0)), temp_channel);
	ASSERT_EQ(model->GetChannel(model->index(2, 0)), two);
	ASSERT_EQ(model->GetChannel(model->index(3, 0)), three);
	ASSERT_EQ(model->GetChannel(model->index(4, 0)), four);
  }
}
