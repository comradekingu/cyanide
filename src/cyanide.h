#ifndef CYANIDE_H
#define CYANIDE_H

#include <QObject>
#include <QtQuick>
#include <thread>
#include <tox/tox.h>
#include <tox/toxav.h>
#include <tox/toxencryptsave.h>

#include "friend.h"
#include "history.h"
#include "message.h"
#include "settings.h"

#define MAX_AVATAR_SIZE 64 * 1024
#define MAX_CALLS 16

const QString TOX_DATA_DIR = QDir::homePath() + "/.config/tox/";
const QString TOX_AVATAR_DIR = TOX_DATA_DIR + "avatars/";
const QString CYANIDE_DATA_DIR = TOX_DATA_DIR + "cyanide/";

const QString DEFAULT_PROFILE_NAME = "tox_save";
const QString DEFAULT_PROFILE_FILE = CYANIDE_DATA_DIR + "default_profile";

const QString DOWNLOAD_DIR = QDir::homePath() + "/Downloads/";

const uint32_t MAX_ITERATION_TIME = 20;

enum LOOP_STATE {
  LOOP_RUN = 0,
  LOOP_FINISH,
  LOOP_RELOAD,
  LOOP_RELOAD_OTHER,
  LOOP_SUSPEND,
  LOOP_STOP,
  LOOP_NOT_LOGGED_IN
};

class Cyanide : public QObject {
  Q_OBJECT

private:
  uint8_t self_address[TOX_ADDRESS_SIZE];

  void kill_tox();
  void killall_tox();
  QString send_friend_request_id(const uint8_t *id, const uint8_t *msg,
                                 size_t msg_length);
  QString send_file_control(int fid, int mid, TOX_FILE_CONTROL file_control);

public:
  Tox *tox;
  ToxAV *toxav;
  Settings settings;
  History history;
  QQuickView *view;

  QString profile_name, next_profile_name;
  uint8_t *tox_save_data;
  size_t tox_save_data_size;
  bool have_password, next_have_password;
  Tox_Pass_Key *tox_pass_key;

  bool save_needed;
  enum LOOP_STATE loop;
  int events;

  Friend self;
  static const int SELF_FRIEND_NUMBER = -1;
  std::map<uint32_t, Friend> friends;

  bool in_call;
  int call_friend_number;
  int call_state;
  int audio_bit_rate, video_bit_rate;
  Q_PROPERTY(bool in_call MEMBER in_call NOTIFY in_call_changed)
  Q_PROPERTY(int call_friend_number MEMBER call_friend_number NOTIFY
                 call_friend_number_changed)
  Q_PROPERTY(int call_state MEMBER call_state NOTIFY call_state_changed)
  Q_PROPERTY(
      int audio_bit_rate MEMBER audio_bit_rate NOTIFY audio_bit_rate_changed)
  Q_PROPERTY(
      int video_bit_rate MEMBER video_bit_rate NOTIFY video_bit_rate_changed)
  std::thread *my_audio_thread;

  Cyanide(QObject *parent = 0);

  /* get the path of the tox save file based on profile_name */
  const QString tox_save_file() const;
  const QString tox_save_file(QString const &name) const;

  uint32_t add_friend(Friend *f);
  uint32_t next_friend_number() const;
  uint32_t next_but_one_unoccupied_friend_number() const;
  void add_message(uint32_t fid, Message &message);
  void incoming_avatar(uint32_t fid, uint32_t file_number, uint64_t file_size,
                       const uint8_t *filename, size_t filename_length);
  void incoming_avatar_chunk(uint32_t fid, uint64_t position,
                             const uint8_t *data, size_t length);
  bool get_file_id(uint32_t fid, File_Transfer *ft);

  /* read the name of the profile (profile_name) to load from
   * DEFAULT_PROFILE_FILE */
  void read_default_profile(QStringList const &args);
  void write_default_profile();

  void load_tox_and_stuff_pretty_please();
  /* loads default name, status, ... */
  void load_defaults();
  /* load name, status, friends from the tox object */
  void load_tox_data();
  void write_save();
  void set_callbacks();
  void set_av_callbacks();
  void do_bootstrap();

  void tox_thread();
  void tox_loop();
  void toxav_thread();
  void audio_thread();

  void suspend_thread();
  void resume_thread();

  void relocate_blocked_friend();
  void send_new_avatar();
  QString send_file(TOX_FILE_KIND kind, int fid, QString const &path,
                    uint8_t *file_id);
  void wifi_monitor();
  void on_message_notification_activated(int fid);

  Q_INVOKABLE bool is_logged_in() const;
  Q_INVOKABLE void raise();
  Q_INVOKABLE bool is_visible() const;
  Q_INVOKABLE void visibility_changed(QWindow::Visibility visibility);
  Q_INVOKABLE void notify_error(QString const summary, QString const body);
  Q_INVOKABLE void notify_message(int fid, QString const summary,
                                  QString const body);
  Q_INVOKABLE void notify_call(int fid, QString const summary,
                               QString const body);
  Q_INVOKABLE void check_wifi();

  Q_INVOKABLE void reload();
  Q_INVOKABLE void load_new_profile();
  Q_INVOKABLE bool load_tox_save_file(QString const path,
                                      QString const passphrase);
  Q_INVOKABLE void delete_current_profile();
  Q_INVOKABLE bool file_is_encrypted(QString const path) const;

  Q_INVOKABLE QString send_friend_request(QString id_string,
                                          QString msg_string);
  Q_INVOKABLE bool accept_friend_request(int fid);
  Q_INVOKABLE void remove_friend(int fid);
  Q_INVOKABLE QString send_friend_message(int fid, QString msg);
  Q_INVOKABLE void send_typing_notification(int fid, bool typing);
  QString send_avatar(int fid);
  Q_INVOKABLE QString send_file(int fid, QString const path);
  Q_INVOKABLE QString resume_transfer(int mid, int fid);
  Q_INVOKABLE QString pause_transfer(int mid, int fid);
  Q_INVOKABLE QString cancel_transfer(int mid, int fid);

  Q_INVOKABLE bool call(int fid, bool video);
  Q_INVOKABLE bool answer();
  Q_INVOKABLE bool hang_up();
  bool call_control(int fid, int control);
  void _callback_call_state(int fid, int state);
  void set_in_call(bool in_call);
  void set_call_friend_number(int call_friend_number);
  void set_call_state(int call_state);
  void set_audio_bit_rate(int audio_bit_rate);
  void set_video_bit_rate(int video_bit_rate);

  void free_friend_messages(Friend &f);
  Q_INVOKABLE void load_history(int fid, QDateTime const from,
                                QDateTime const to);
  Q_INVOKABLE void clear_history(int fid);
  Q_INVOKABLE QDateTime null_date() const;

  /* setters and getters */
  Q_INVOKABLE QString get_profile_name();
  Q_INVOKABLE QString set_profile_name(QString name);

  Q_INVOKABLE QList<int>
  get_friend_numbers() const; /* friend list ordering goes here */
  Q_INVOKABLE QList<int> get_message_numbers(int fid) const;

  Q_INVOKABLE void set_friend_activity(int fid, bool status);
  Q_INVOKABLE void set_friend_blocked(int fid, bool block);
  Q_INVOKABLE void set_self_name(QString const name);
  Q_INVOKABLE void set_self_status_message(QString const status_message);
  Q_INVOKABLE void set_self_user_status(int status);
  Q_INVOKABLE QString set_self_avatar(QString const path);
  Q_INVOKABLE void set_random_nospam();

  Q_INVOKABLE QString get_self_address() const;
  Q_INVOKABLE int get_self_user_status() const;

  Q_INVOKABLE QString get_friend_public_key(int fid) const;
  Q_INVOKABLE QString get_friend_name(int fid) const;
  Q_INVOKABLE QString get_friend_avatar(int fid) const;
  Q_INVOKABLE QString get_friend_status_message(int fid) const;
  Q_INVOKABLE QString get_friend_status_icon(int fid) const;
  Q_INVOKABLE QString get_friend_status_icon(int fid, bool online) const;
  Q_INVOKABLE bool get_friend_connection_status(int fid) const;
  Q_INVOKABLE bool get_friend_accepted(int fid) const;
  Q_INVOKABLE bool get_friend_blocked(int fid) const;

  Q_INVOKABLE int get_message_type(int fid, int mid) const;
  Q_INVOKABLE bool get_message_author(int fid, int mid) const;
  Q_INVOKABLE QString get_message_text(int fid, int mid) const;
  Q_INVOKABLE QString get_message_html_escaped_text(int fid, int mid) const;
  Q_INVOKABLE QString get_message_rich_text(int fid, int mid) const;
  Q_INVOKABLE QDateTime get_message_timestamp(int fid, int mid) const;
  Q_INVOKABLE int get_file_status(int fid, int mid) const;
  Q_INVOKABLE QString get_file_link(int fid, int mid) const;
  Q_INVOKABLE int get_file_progress(int fid, int mid) const;

signals:
  void signal_focus_friend(int fid);
  void signal_friend_added(int fid);
  void signal_friend_activity(int fid);
  void signal_friend_blocked(int fid, bool blocked);
  void signal_load_messages();

  void signal_friend_request(int fid);
  void signal_friend_message(int fid, int mid, int type);
  void signal_friend_action();
  void signal_friend_name(int fid, QString previous_name);
  void signal_friend_status_message(int fid);
  void signal_friend_status(int fid);
  void signal_friend_typing(int fid, bool is_typing);
  void signal_friend_read_receipt();
  void signal_friend_connection_status(int fid, bool online);
  void signal_avatar_change(int fid);

  void signal_file_status(int fid, int mid, int status);
  void signal_file_progress(int fid, int mid, int progress);

  void signal_call(int fid);

  void in_call_changed();
  void call_friend_number_changed();
  void call_state_changed();
  void audio_bit_rate_changed();
  void video_bit_rate_changed();

public slots:
  void wifi_changed(QString &str, QDBusVariant &variant);
};

//: default username
const QString DEFAULT_NAME = Cyanide::tr("Tox User");
//: default status
const QString DEFAULT_STATUS = Cyanide::tr("Toxing on Cyanide");
//: default friend request message - feel free to tranlate freely
const QString DEFAULT_FRIEND_REQUEST_MESSAGE = Cyanide::tr("Tox me maybe?");

#endif // CYANIDE_H
