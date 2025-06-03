## 整体规划
- 采用Teambition进行任务的分配和管理
- 使用敏捷开发，以一个题目作为一个 **User Story**，每个人按需领取 **User Story**，独立完成需求分析、代码实现、测试、交付。
- 采用 **git** 进行版本控制，每个 feature 一个分支，以 **develop** 作为主线开发分支

## 题目实现思路

### basic

miniob的基础功能

### aggregation-func

实现聚合函数 max/min/count/avg/sum。

思路：使用一个hashmap来保存key（group by的tuple, 没有group by则使用一个默认的key）到value（要聚合的tuple）的映射，通过Combine将相同key的tuple聚合起来。

注意：
- 要特别处理一下NULL，count(*) 是要算NULL的，普通count不需要算NULL，其他聚合函数也不需要算NULL。
- 不一定只有select里面的要聚合，where里面可能也有聚合，这些也要纳入聚合计算里面。
- select list里面出现了聚合函数，就不能再包含除了group by之外的字段。

### like

思路：扩展condition，比如原来是 a < b，现在可以 a like b。具体如何实现模式匹配，完全可以交给GPT。

### sub-query && complex-sub-query

支持子查询。实现起来有点复杂，相对于重构了一遍。

可以复用原本的sql解析逻辑，将子查询当成一个相对独立的sql。原本只用解析一个sql，现在需要解析一颗树。

注意要先解析父sql，这样可以记录父sql里面的表，这样在处理子sql时就能看到父sql的表。

这里是把子查询实现成了一种Expression，后续还把（a,b,c）这种集合也实现成了Expression，这样能比较容易地嵌入原有的逻辑。

这样上层需要向下层要数据的时候，调用Expression的try_get_value接口，不需要关心是不是子查询，如果是一个子查询，直接把之前解析得到的物理算子执行一遍就好了。

这套Expression的抽象还是挺好的，后续update-select可以直接复用。

### function

注意到这里的function都是作用与单个字段的，所以直接在语法解析时给node加上function信息就行了，在FieldExpr里面特判一下，有function就再执行一下就行了。

### update-select

这里直接复用sub-query的框架，但是在一个bug上卡了很久，后来发现这是因为 `update ... set x=(select ...) where ...` 这种如果select能出来多个tuple，本来应该是fail的，但是where有可能会筛掉所有数据，这种情况还是SUCCESS，解决方法就是特判一下。（实际上这里应该在真正执行update的时候再去执行查询的，但是因为解析的时候要类型，而我们没有schema，只能先执行了）

### expression

原来只能查询字段，现在支持查询表达式。

这里是**全面使用Expression这套框架**了，这套框架贯穿了整个查询。原来的where也只支持字段，现在也可以扩展为Expression，子查询也是实现成了Expression，因为Expression有一个try_get_value接口，所以交互起来比较便利。

因为高度依赖表达式树，还实现了一个接口用来方便地遍历表达式树。

### alias

- table_map里面要多加alias到table的映射。
- 记录alias到真实名字的映射，细节东西比较多。

### group-by

在实现聚合函数的时候就实现了。

### create table select

如果create table 的时候带了select，那么就在创建完表后直接插入数据，这里同样是复用了Expression那套框架！

### create view

如果要真的实现的话比较复杂。这里直接hack！

想着create view和create table挺像，是不是可以直接复用，果然可以：

- 如果要create view，就记录下来view name到sql的映射
- 每次（对，就是每次）执行sql之前，按顺序drop table，再create table select ...
- 因为view是真实的table，所以天然支持查询
- 如果要更新或删除，那么就把 view name 直接替换为真实的 table 的 name，这样更新或删除的就是真实的那个table了。



### drop table

大概是最简单的一个题了，只需要把create table给逆向操作就可以了，官网也给出了详细的指导，大致思路分为三部分：

1. 删除`xxx.table` 表的元数据文件
2. 删除`xxx.data`数据文件
3. 删除`xxx.index`索引
除此之外做一些简单的异常处理就可以了，校验一下表名是否存在等。

### Date

今年miniob的代码结构相比于22年要优化了不少，但是Date涉及到要改动的地方依旧不少，主要有：

1. 在lex和yacc当中添加对 date和日期格式的匹配支持
2. 在value当中添加date类型和表示形式
3. 考虑Date的具体存储方法
4. 为Date添加初始化，校验，比较等功能
**解析**
主要需要添加两个类型的解析，一个是`date`关键字，一个是`yyyy-mm-dd`格式的字符串匹配，前者直接定义一个token即可，后者可以采用正则的形式来处理，对于日期字符串的正则匹配，需要注意的是范围需要从`0000-00-00`到`9999-99-99`都匹配上，否则如13月这种错误的日志就会匹配到普通字符串上，从而导致后期无法进行校验处理。其他的按照官方手册实现即可
**存储与表示**
在存储上，我选择了使用一个yyyymmdd的int32来进行存储这样只需要四个字节，比字符串要节省空间。
在内存表示上，我选择了定义一个Date类，并将相关的相关的逻辑操作都封装在这里面，如类型转换，合法性校验，比较，to_string等。

```cpp
class Date
{
public:
  Date() = default;
  Date(const char *str);
  Date(int data);
  // Date(int year,int month,int day);

  bool        isValid() const { return valid; }
  bool        operator<(const Date &that) const;
  bool        operator==(const Date &that) const;
  int         compare(const Date &that) const;
  std::string to_string() const;
  int         to_int() const;
  void        parse_date_int(int date);
  bool        isDateValid() const;
  bool        valid{};
  int         year{};
  int         month{};
  int         day{};
  std::string format(const std::string &formatStr) const；
};

```

### Value

在value当中，添加了一个Date字段来存储一个Date类，并且把转换为int的date存放到num_value_当中，之后.data()用来传输数据和持久化时直接返回这个num_value_即可。
到这date基本就实现完了，似乎今年miniob的代码重构了不少，往年说还需要更改B+树部分的今年都不需要了。

### typecast

说Join之前先说一下typecast，今年typecast没有作为一个单独的题目出现，但是也并没有给提供很完善的实现，在实现join时发现了join对typecast强依赖，涉及到很多int和string，float和string之间的join。主要需要为string和其他的类型之间提供转换。
在github的wiki当中给出了标准，照着实现就好

> 字符串转数字
> 
- 如果字符串刚好是一个数字，则转换为对应的数字（如'1'转换为1，'2.1'转换为2.1）;
- 如果字符串的前缀是一个数字，则转换前缀数字部分为数字（如'1a1'转换为1，'2.1a'转换为2.1）;
- 如果字符串前缀不是任何合法的数字，则转换为0（不需要考虑前导符号 '+' '-'）； **问题：**
- 如果转换数字溢出怎么处理?（不考虑）
- 是否考虑十六进制/八进制?（不考虑）

### Join Tables

算是mid里面最简单的几个之一了，但是依赖不少其他的相关内容，如typecast和expression，今年的miniob已经实现了select tables，因此只需要再次基础上实现join即可。
先从解析开始说，join主要分为三部分，左表，右表，和条件，通过inner join on来切分即可，并且由于可能存在多张表之间的join，因此这个解析为一个递归的过程，可以先递归的解析inner join 后面的内容，之后再将最初的左表添加到其中。在条件的解析上，队友定义了conditionTree的解析方式，可以很方便的解析多个condition，我这里就直接拿来用了。解析的格式如下：

```
inner_join_list:
| INNER JOIN table_name ON condition_tree inner_join_list

```

最终的解析结果可以得到n个table和n-1个conditon_tree，之后在select_stmt的create当中按照和filter_stmt相同的方法来创建一个join_stmt即可。
最后改写一下join_operator当中的逻辑即可，这一部分反而是工作量最小的。
23年的数据量并不大，并不存在前两年的6表join的情况，因此只需要实现最基础的NLJ，甚至我们在重构expression的时候取消了谓词下推依旧可以通过测试。
**bug**
在调试过程中，发现了一个miniob当前存在的bug，join operator当中在匹配时，每当右表匹配完一轮之后都要调用table_scan的close()和open()函数，而在这其中，open()当中又会调用一个set schema的函数，在这其中，是采用push_back的方式进行初始化的，但是在close()当中又没有清空掉field数组，就导致，每次open，都会向里面重新添加一遍field，从而导致在进行多轮遍历之后，tuple里面会有几十个field之多，不过在物化的过程中，只会根据record去获取value，这样并不会影响正确性，但是会导致平白无故的添加一堆无用的数据，我在print tuple.to_string时都惊了，也可能正是因为不影响正确性，这个bug也没有被发现，正确的做法应该是在set_schema时先重置再添加

```cpp
  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.clear();
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new FieldExpr(table, &field));
    }
  }

```

### Update

update比较简单，先删后增即可，注意在big query和big write当中需要支持update多个字段，其他的参照delete实现即可，应该是最简单operator了，用来熟悉一下整个sql的流程不错。

### big write / query

update加上多字段支持之后直接过掉big query和big write，直接+60，恰分恰爽了。

### null

参考往年的思路，选择在sys_field后添加一个null_field来表示null(个人感觉单纯用一个magic number表示null也不是不行，单纯想过test应该是可以过的)。
null_field为一个长度为32的bitmap，可以表示后面32个field是否为null，完全足够，在存储的时候直接使用一个4个字节的chars进行存储即可。
null有这么2个问题：

1. update时如果从index = 0开始物化value，会读到null，之后再make_record时就会把null又给扔进去当成了一个普通的field，导致record里面有两个null_field，需要给一个sys_field_num + null_field_num的一个offset，另外一种解决方法就是重写cell_at函数，在其中给一个offset，不对外暴露sys_field和null_field
2. null的设置时机，刚从磁盘当中读取出来的record是不具有null属性的，需要在一个合适的时机通过bitmap来恢复其null属性，最开始定义在project operator里面，后面发现有很多依赖null的operator在project operator下面，因此把这个设置null的过程移动到了RowTuple到set_record和cell_at里面，在set_record时，从中解析出bitmap，之后cell_at时根据bitmap去判断是否为null，是否允许为null到逻辑也在这里处理，不过既然能正确写入，这里一般是不会出问题的
此外，null还需要Field和Value为其提供支持，添加字段来表示是否允许为null和是否为null，总之null需要改动的部分非常多，当时commit时看了一眼大概修改了20个文件。。麻了
今年null的测试依赖也很多，需要实现了update-select，和unique之后才能过，而unique又依赖multi index。

### text

逻辑上比较简单，但是坑不少。
逻辑上采用溢出页的方式处理比较简单，写入时按需额外去申请几个page用来存储text数据，record本身存储page_num，读取时再根据page_num去获取到真实的text内存。
今年的要求变了，text的最大长度变成了和MySQL一致的64KB，但是本身的PAGE_SIZE也从4KB变成了8KB，因此如果算上header的话，需要9个溢出页来进行存储，最长的情况为前8个page全部存满，最后一个page存部分。
因此record当中的field就需要9个int来存储，整个field占36个字节，没有用上的page在存储时就向record里面添加INVALID_PAGE_NUM，读取的时候读取到INVALID_PAGE_NUM就截断即可。
但是实现的时候在长度上有两个问题，一个是obclient无法接受超过4KB的text，超长会截断，这个需要下载readline，之后重新编译，还有一个是发送网络请求时的buffer size为8KB，全文搜索长度，之后在后面加个0就行。
在进行删除和更新上，需要首先删除溢出页当中的内容，之后再删除掉该条record，删除的话可以有两种思路，一是将该page通过`init_empty_page`进行初始化，之后加入到free_list当中，而是可以使用dispsoe将其删除，之后用来重新分配。不过我都没实现，谁让不检测呢 :)

### Update mvcc

先说说MVCC本身，多版本并发控制，但是严格意义上来说并不是一个完整的并发控制协议，MVCC最大的特点是保证了读写之间不冲突，写请求不会因为正在读而阻塞写入，而读请求可以根据自身版本去读取到对自己可见的版本数据，不会被正在执行的写入阻塞。
之所以说MVCC并不是一个完整的并发控制，是因为MVCC并没有办法解决写写之间的冲突，因此需要其他的手段来加以辅助，常见的手段有基于时间戳或者基于锁的，Miniob当中的实现方式比较粗暴，只要有两个事务在更新同一个record，那么后一个事务在更新时就返回failture。
而MVCC当中的版本是基于tid的，在开启了MVCC模式之后，每一条record会生成两个sys_field，分别存储begin和end，来标识一个事务的可见性，这里似乎并没有一个统一的标准，只要能满足MVCC协议本身的要求即可，在Miniob当中的设置如下：
record通过begin和end 两个id进行状态管理，begin用于表示事务开始的时间，end为事务结束的时间，对于一条record,当一个事务开始时，新的record的begin设置为-trx_id，end为max_int32,表示事务写入但未提交，而删除时则将end设置为-trx_id，表示删除但未提交，写入操作提交时，将begin设置为trx_id，删除操作提交时将end设置为trx_id，最终会产生六种状态：

其中，已提交指的就是当前事务已经结束，自然不存在什么可见不可见的问题。而Miniob当中的隔离级别应当是serializable，因为未提交的事务的 begin < 0，因此是永远无法读取到新写入的record，因此是不存在幻读情况的。

| begin_xid | end_xid | 自身可见 | 其他事务可见 | 说明 |
| --- | --- | --- | --- | --- |
| -trx_id | MAX | 可见 | 不可见 | 由当前事务写入，但还未提交，对其他事务不可见 |
| trx_id | MAX | 已提交 | 对新事务可见 | 写入并且已经提交，对之后的新事务可见 |
| trx_id | 正数 | 已提交 | 旧事务不可见，新事务可见 | 在当前事务中进行写入，并且事务提交 |
| 任意正数 | trx_id | 已提交 | 旧事务可见，新事务不可见 | 在当前事务中进行删除，并事务提交 |
| 任意正数 | -trx_id | 不可见 | 可见 | 在当前事务中进行删除，未提交 |
| -trx_id | -trx_id | 不可见 | 不可见 | 由当前事务进行写入，并且又被当前事务删除，并且未提交 |

```cpp
if (begin_xid > 0 && end_xid > 0) {
  if (trx_id_ >= begin_xid && trx_id_ <= end_xid) {
    rc = RC::SUCCESS;
  } else {
    rc = RC::RECORD_INVISIBLE;
  }
} else if (begin_xid < 0) {
  // begin xid 小于0说明是刚插入而且没有提交的数据
  rc = (-begin_xid == trx_id_) ? RC::SUCCESS : RC::RECORD_INVISIBLE;
} else if (end_xid < 0) {
  // end xid 小于0 说明是正在删除但是还没有提交的数据
  if (readonly) {
    // 如果 -end_xid 就是当前事务的事务号，说明是当前事务删除的
    rc = (-end_xid != trx_id_) ? RC::SUCCESS : RC::RECORD_INVISIBLE;
  } else {
    // 如果当前想要修改此条数据，并且不是当前事务删除的，简单的报错
    // 这是事务并发处理的一种方式，非常简单粗暴。其它的并发处理方法，可以等待，或者让客户端重试
    // 或者等事务结束后，再检测修改的数据是否有冲突
    rc = (-end_xid != trx_id_) ? RC::LOCKED_CONCURRENCY_CONFLICT : RC::RECORD_INVISIBLE;
  }
}

```

对于record是否可见的判断，在visit_record当中提供了一段代码，

**当前存在的问题**

1. 缺少begin < 0 && end < 0的处理，导致当前事务中写入的记录无法删除
2. commit时存在问题，insert之后，delete的记录无法添加到operation里面，operation为一个set,但是insert时似乎无法正确添加，会导致即便修复了1, 在commit之后会出现begin = commit_id,end = -trx_id的情况，理论上这种情况不应该出现，因为delete的commit行为没有得到正确执行，如果delete的commit行为得到正确执行，就会变成begin > 0 && end > 0的情况，就可以按照版本去读取了。这里我也不知道该怎么修复了。。因为set的行为有点奇怪
    
    ![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/20231105124219.png)
    
    ![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/20231105124249.png)
    
    ![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/20231105124421.png)
    
    ![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/20231105124430.png)
    

不过其中存在一定的问题，即少了对begin < 0 && end < 0的判断，在这种情况下，为当前事务写入，又被当前事务删除，但是事务还未提交，此时应当对当前事务是不可见的，但是根据目前的条件结构，会返回SUCCESS，这样会导致在当前事务中进行delete但是仍可见的问题，或者当前事务中进行update，但是旧数据还存在的问题。需要补充一条逻辑，在这种情况下返回不可见
如下面的例子：
原本(4,4,'a')的record是由上一个事务写入的，并且提交(begin_xid > 0)，此时更新就不会出现问题，而(4,761,'a')的record是当前事务写入的，还为提交，begin_xid < 0,删除之后end_xid < 0，上面漏了对这种情况的判断，从而导致其又可见。
即便如此，依旧还存在其他的问题，即如果当前的事务commit之后，这条记录就又可见了

![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/20231105004447.png)

下图是我用初始版的miniob做的测试，可以确认的确存在这个问题

![](https://pic-bed-1309931445.cos.ap-nanjing.myqcloud.com/blog/c6ff7993fbfb139743ff5b650a79a760.png)

回到正题，update-mvcc这题其实是不怎么需要实现的，大多数人在完成了unique之后，update-mvcc是可以直接通过的，因为目前miniob已经给出了mvcc的insert和delete实现，组合一下即可。不过由于我们队unique index实现的是一个fake版本，在mvcc模式下并没有办法提供唯一性保证，而我又不想去重构，因此我们队分了两天线进行，队友去重构unique index，我特判表名然后实现一个内存当中的mvcc版的unique index，其实就是把visit_record当中的逻辑对着unique_index实现了一遍，不过最后是我先搞出来了hhh。

```cpp
RC Table::insert_record(Record &record)
{
  int decode_index_num;
  if (table_meta_.name() == mvcc_table_name_) {
    int index_offset = table_meta_.sys_field_num() + table_meta_.null_field_num();
    memcpy(&decode_index_num,record.data() + index_offset * 4,4);
    if (mvcc_unique_index_.contains(decode_index_num)) {
      if (is_duplicated(decode_index_num)) {
        return RC::INVALID_ARGUMENT;
      }
    }
  } else {
  // ...
    }
}

bool Table::is_duplicated(int index_num) {
  if (!mvcc_unique_index_.contains(index_num)) {
    return false;
  }
  auto current_index = mvcc_unique_index_[index_num];
  int begin_tid = current_index.first;
  int end_tid = current_index.second;
  bool is_visible;
  bool is_conflict;
  if (begin_tid > 0 && end_tid > 0) {
    if (trx_->id() >= begin_tid && trx_->id() <= end_tid) {
      is_visible = true;
    } else {
      is_visible = false;
    }
  } else if (begin_tid < 0) {
    if (-begin_tid == trx_->id() && end_tid < 0) {
      is_visible = false;
    } else if (-begin_tid == trx_->id()) {
      is_visible = true;
    } else {
      is_visible = false;
    }
  } else if (end_tid < 0) {
    // 由于unique index只需要管理插入，因此不存在read_only的情况
    if (-end_tid == trx_->id()) {
      is_visible = false;
    } else {
      is_conflict = true;
    }
  }
  // 正在有其他的尝试插入，直接拒绝
  // 如果存在一条先前可见的数据，也拒绝掉
  if (is_conflict || is_visible) {
    return true;
  }
  return false;

}

```

在完成了unique index之后还存在一个问题，就是mvcc模式下,update时，下层的table_scan会把新写入的record又给读出来，从而导致又insert一遍，再加上我最开始visit_record当中begin < 0 end < 0的bug没修，从而导致其会逃过unique的限制，表现结果就是一个update更新了两次。
问题的源头是table_scan会读取到新写入的record，不过当时没仔细看到底为什么，只是修了visit_record和在update当中加了一层限制，即如果原记录和新记录完全一致的话，就不进行更新，这样勉强算是把窟窿给堵上了，最后也是通过了测试。
