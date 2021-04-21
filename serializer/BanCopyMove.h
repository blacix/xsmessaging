#ifndef NOWTECH_BAN_COPY_MOVE_INCLUDED
#define NOWTECH_BAN_COPY_MOVE_INCLUDED

namespace nowtech
{

/// Class to inhibit copy or move construction or initialization of the subclasses.
class BanCopyMove
{
public:
	BanCopyMove() = default;
	BanCopyMove(const BanCopyMove &other) = delete;
	BanCopyMove(BanCopyMove &&other) = delete;
	BanCopyMove &operator=(const BanCopyMove &other) = delete;
	BanCopyMove &operator=(BanCopyMove &&other) = delete;
};

} // namespace nowtech

#endif // NOWTECH_BAN_COPY_MOVE_INCLUDED
