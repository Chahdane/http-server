import 'dart:convert';
import 'dart:math' as math;

import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:intl/intl.dart';
import 'package:timeago/timeago.dart' as timeago;
import '../../flutter_flow/lat_lng.dart';
import '../../flutter_flow/place.dart';
import '../../flutter_flow/custom_functions.dart';
import '../../backend/backend.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import '../../auth/auth_util.dart';

bool? searchFunction(
  String? needleName,
  String haystackFName,
  String haystackLName,
  String city,
  String? needleCity,
  String? needleJob,
  String? job,
) {
  String sum = haystackFName + haystackLName;

  if (needleCity == null || needleCity == "all") {
    if (needleJob != null && job != null) {
      if (job.toLowerCase().contains(needleJob.toLowerCase())) {
        if (needleName == null) {
          return true;
        } else {
          if (sum.toLowerCase().contains(needleName.toLowerCase())) {
            return true;
          } else {
            return false;
          }
        }
      }
    }
  } else if (needleJob == null || needleJob == "all") {
    if (city.toLowerCase().contains(needleCity.toLowerCase())) {
      return true;
    }
  }

  if (needleName != null) {
    if (sum.toLowerCase().contains(needleName.toLowerCase())) {
      if (needleCity == null || needleCity == "all") {
        if (needleJob == null || needleJob == "all") {
          return true;
        }
      } else {
        if (city.toLowerCase().contains(needleCity.toLowerCase()) ||
            needleCity == "all") {
          if (job == null) {
            return true;
          } else {
            if (needleJob != null) {
              if (job.toLowerCase().contains(needleJob.toLowerCase())) {
                return true;
              } else {
                return false;
              }
            }
          }
        } else {
          return false;
        }
      }
    }
  }

  return false;
}
