<?php
// $Id$
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

// Style editor
function style_edit_styleset_edit_form($formstate, $theme, $styleid) {

  // Form with values for text editor
  $form = set_text_editor_style_form_values(outline_text_edit_styleset(), $styleid);

  set_styleset_edit_breadcrumb($styleid);
  return $form;
}
/**
 * Implementation of hook_submit()
 *
 * Update the database with the new values
 *
 * @param $form array
 *   Current form
 * @param $form_state array
 *   Submit state from the user
 */
function style_edit_styleset_edit_form_submit($form, &$form_state) {
  $val = $form_state['values'];
  switch ($val['sel_cat']['kind']) {
    case 'STYLE':
      $update_val = array($val['sel_cat']['selector'],'{'.$val['style']['css'].'}' ,$val['styleid']);
      break;
    case 'COMMENT':
      $update_val = array($val['sel_cat']['selector'],$val['style']['css'] ,$val['styleid']);
      break;
    default:
      drupal_set_message(t('Type not known!'), 'error');
      break;
  }
  db_query("UPDATE {style_edit_cssstyles} SET selector = '%s', style = '%s' WHERE styleid = %d",
  $update_val);
  set_stylesheet_modified($val['cssid']);
  drupal_set_message(t('Saved style ID @sid', array('@sid' => $val['styleid'])));

  $form_state['redirect'] = redirect_to_stylescheet($val['cssid']);
}

function redirect_to_stylescheet($cssid) {
  $sql = 'SELECT * FROM {style_edit_cssimport} WHERE cssid = %d';
  $result = db_query($sql, $cssid);
  $cssinfo = db_fetch_array($result);

  return 'admin/settings/styleedit/list/'
    . $cssinfo['systemname'] . '/editsheet/' . $cssid; 
}

function set_stylesheet_modified($cssid) {
  db_query('UPDATE {style_edit_cssimport} SET udbtimestamp = %d WHERE cssid = %d',
  array(time(),$cssid));
}

function set_styleset_edit_breadcrumb($styleid) {
  
  $cssid = db_result(db_query('SELECT cssid FROM {style_edit_cssstyles} WHERE styleid = %d',$styleid));
  
  $sql = 'SELECT * FROM {style_edit_cssimport} WHERE cssid = %d';
  $result = db_query($sql, $cssid);
  $cssinfo = db_fetch_array($result);
  
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Style Editor'), 'admin/settings/styleedit');
  $breadcrumb[] = l($cssinfo['name'], 'admin/settings/styleedit/list/' . $cssinfo['systemname']);
  $breadcrumb[] = l($cssinfo['filename'], 'admin/settings/styleedit/list/' .
    $cssinfo['systemname'] . '/editsheet/' . $cssid);
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Sets the values from data base for the text editor
 *
 *  @param  $form array
 *    formoutline of text editor
 *  @param  $styleid int
 *    record id to load from 'style_edit_cssstyles'
 *  @return array
 *    form with values
 */
function set_text_editor_style_form_values($form, $styleid) {
  $data = get_stylesection_db($styleid);

  $form['sel_cat']['selector']['#default_value'] = $data['selector'];
  $form['sel_cat']['kind']['#default_value'] = $data['kind'];
  $form['style']['css']['#default_value'] = clear_brackets($data['style']);
  $form['styleid']['#value'] = $data['styleid'];
  $form['cssid']['#value'] = $data['cssid'];
  $form['edit_type']['#value'] = variable_set('style_edit_editor_type','text');

  return $form;
}

function outline_text_edit_styleset() {

  $form = array();

  $form['sel_cat'] = array(
            '#type' => 'fieldset',
            '#title' => t('Selector and category'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
  );

  $form['sel_cat']['selector'] = array(
            '#type' => 'textfield',
            '#title' => t('Selector'),
  );

  $form['sel_cat']['kind'] = array(
            '#type' => 'select',
            '#title' => t('Type'),
            '#options' => array(
                'COMMENT' => t('Comment'),
                'STYLE' => t('Style'),
  ),
  );

  $form['style'] = array(
            '#type' => 'fieldset',
            '#title' => t('Styles'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
  );

  $form['style']['css'] = array(
            '#type' => 'textarea',
            '#title' => t('CSS'),
            '#rows' => 10,
  );

  $form['styleid'] = array(
            '#type' => 'value',
  );

  $form['cssid'] = array(
            '#type' => 'value',
  );

  $form['edit_type'] = array(
            '#type' => 'value',
  );

  $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
  );

  return $form;
}

/**
 *  Load the Stylesheet from the database
 *
 * @param $cssid int
 *   ID of the stylesheed to load
 * @return array
 *   List of items and count field
 */
function get_stylesheet_db($cssid) {
  $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE cssid = %d ORDER BY weight';
  $result = db_query($sql,$cssid);
  $table = array();
  while ($r = db_fetch_array($result)) {
    $table[] = $r;
  }
  $table['count'] = $result->num_rows;
  return $table;
}

//Stylesheet Form
/**
 * Operation types
 *
 * @return array
 *   Operation types
 */
function styleset_ops() {
  $styleset_ops = array(
        'Edit' => 'edit',
        'Delete' => 'delete',
        'Copy' => 'copy',
        'Add' => 'add',
  );
  return $styleset_ops;
}

function style_edit_stylesheet_edit_form($formstate, $theme, $cssid) {
  $styles = get_stylesheet_db($cssid);

  //Check for unsaved styles
  $result = db_query('SELECT * FROM {style_edit_cssimport} WHERE cssid = %d',$cssid);
  $cssinfo = db_fetch_array($result);
  if ($cssinfo['ufiletimestamp'] < $cssinfo['udbtimestamp'])
  drupal_set_message(t('You have unsaved changes!'), 'warning');

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Style Editor'), 'admin/settings/styleedit');
  $breadcrumb[] = l($cssinfo['name'], 'admin/settings/styleedit/list/'.$theme);
  drupal_set_breadcrumb($breadcrumb);

  $form = array();

  $form['styleitems'] = array();
  $form['styleitems']['#tree'] = TRUE;

  $itemcount = $styles['count'];

  foreach ($styles as $key => $style) {

    if (count($style) < 2) continue;

    $sel = $style['selector'];
    $css = escape_html($style['style']);
    $sort = $style['weight'];
    $sid = $style['styleid'];

    $form['styleitems'][$key]['selector'] = array(
            '#value' => $sel,
    );
    $form['styleitems'][$key]['style'] = array(
            '#value' => $css,
    );

    foreach (styleset_ops() as $op) {
      $form['styleitems'][$key][$op.'_op'] = array(
                '#value' => l(t($op),'admin/settings/styleedit/styleset/'
                . $theme . '/' . $op . '/' . $sid),
                );
    }

    $form['styleitems'][$key]['sort'] = array(
            '#type' => 'weight',
            '#delta' => $itemcount,
            '#default_value' => $sort,
    );
    $form['styleitems'][$key]['sid'] = array(
            '#input' => 'value',
            '#value' => $sid,
    );
  }

  $form['cssid'] = array(
        '#type' => 'hidden',
        '#value' => $cssid,
  );

  $form['submit_bottom'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
  );

  $form['submit_top'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
  );

  return $form;
}

function style_edit_stylesheet_edit_form_submit($form, &$form_state) {
  //Update the database with the new values'
  foreach ($form_state['values']['styleitems'] as $item) {
    db_query('UPDATE {style_edit_cssstyles} SET weight = %d WHERE styleid = %d LIMIT 1',
    array($item['sort'], $item['sid']));
  }

  drupal_set_message(t('Saved stylesheet to database'));

  $time = save_stylesheet_to_fs($form_state['values']['cssid']);

  db_query('UPDATE {style_edit_cssimport}
        SET udbtimestamp = %d, ufiletimestamp = %d
        WHERE cssid = %d',
  array($time,$time,$form_state['values']['cssid']));
}

function theme_style_edit_stylesheet_edit_form($form){

  $output = drupal_render($form['submit_top']);

  drupal_add_tabledrag('css_table', 'order', 'sibling', 'lsort');
  $header = array(
        '',
  t('Selector'),
  t('Style'),
  array('data' => t('Operation'), 'field' => 'op', 'colspan' => count(styleset_ops()), 'sort' => '',),
        'sort',
  );

  foreach (element_children($form['styleitems']) as $key) {

    $form['styleitems'][$key]['sort']['#attributes']['class'] = 'lsort';
    $form['styleitems'][$key]['itemid']['#attributes']['class'] = 'itemid';

    $row = array('');
    $row[] = drupal_render($form['styleitems'][$key]['selector']);
    $row[] = drupal_render($form['styleitems'][$key]['style']);
    foreach (styleset_ops() as $op) {
      $row[] = drupal_render($form['styleitems'][$key][$op.'_op']);
    }
    $row[] = drupal_render($form['styleitems'][$key]['sort']).
    drupal_render($form['styleitems'][$key]['sid']);

    //Add a draggable class to every table row (<tr>)
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }

  $output .= theme('table', $header, $rows, array('id' => 'css_table'));

  $output .= drupal_render($form);

  return $output;
}

/**
 *  Load the Stylesheet from the database
 *
 * @param int $styleid
 *   ID of the stylesheed section to load
 * @return array
 *   List of items
 */
function get_stylesection_db($styleid) {
  $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE styleid = %d ';
  $result = db_query($sql,$styleid);
  return db_fetch_array($result);
}

/**
 * Cleans style on '{''}'
 *
 * @param $text string
 *   Text to clean
 * @return string
 *   Clean text
 */
function clear_brackets($text) {
  $text = str_replace('{', '', $text);
  $text = str_replace('}', '', $text);
  return $text;
}

/**
 *makes html visible
 *
 * @param string $text
 *   Text to clean
 * @return string
 *   Clean text
 */
function escape_html($text) {
  $text = str_replace('<', '&lt;', $text);
  $text = str_replace('>', '&gt;', $text);
  return $text;
}

function save_stylesheet_to_fs($cssid) {

  $sql = "SELECT * FROM {style_edit_cssimport} WHERE cssid = %d";
  $result = db_query($sql,$cssid);
  $cssrow = db_fetch_array($result);

  $stylesections = get_stylesheet_db($cssid);

  $cssfile = $cssrow['path'];

  if (is_writable($cssfile)){
    $fhandle = fopen($cssfile, 'w+');
    foreach ($stylesections as $section) {

      if (count($section) < 2) continue;

      fwrite($fhandle, $section['selector']);
      fwrite($fhandle, $section['style']);
      fwrite($fhandle, chr(10).chr(13));  // change to "\n"
    }
    fclose($fhandle);
    drupal_set_message(t('Sucsessfull writen @file',array('@file' => basename($cssfile))));
  }else {
    drupal_set_message(t('Error while writing @file',array('@file' => basename($cssfile))));
  }

  return filemtime($cssrow['path']);
}


// Settings

/**
 * Implementation of style_edit_settings_form().
 *
 * @return array
 *   An array containing the form elements to be displayed in the node edit form.
 */
function style_edit_settings_form($formstate) {
  $form = array();

  $form['edit_setting'] = array(
          '#type' => 'fieldset',
          '#title' => t('Editor settings'),
          '#tree' => TRUE,
          '#collapsible' => TRUE,
  );
  $form['edit_setting']['editor_type'] = array(
          '#type' => 'select', 
          '#title' => t('Select style editor type'), 
          '#default_value' => variable_get('style_edit_editor_type','text'),
          '#options' => array(
              'text' => t('Text editor'), 
              'assisted' => t('Formular based editor'), 
  ),
          '#description' => t('Global setting for the syleset editor type.'),
  );

  $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
  );

  return $form;
}

/**
 * Implementation of style_edit_settings_form_submit().
 *
 */
function style_edit_settings_form_submit($form, &$form_state) {
  $val = $form_state['values'];
  variable_set('style_edit_editor_type','text');
  drupal_set_message(t('Settings saved'));
  drupal_goto('admin/settings/styleedit/list/');
}


/**
 * Implementation of hook_form().
 *
 * @param &$node The node being added or edited.
 * @param $form_state The form state array.
 * @return An array containing the form elements to be displayed in the node edit form.
 */
function style_edit_styleset_delete_form($form_state, $theme, $styleid) {
  $styleset = get_stylesection_db($styleid);
  
  set_styleset_edit_breadcrumb($styleid);
  
  $form = array();
  $form['style'] = array(
            '#type' => 'fieldset',
            '#title' => t('Style section'),
            '#tree' => TRUE,
            '#collapsible' => FALSE,
  );

  $form['style']['selector'] = array(
            '#type' => 'markup',
            '#title' => t('Selector'),
            '#value' => $styleset['selector'],
  );
  $form['style']['css'] = array(
            '#type' => 'markup',
            '#title' => t('CSS'),
            '#value' => $styleset['style'],
  );
  $form['styleid'] = array(
            '#type' => 'value',
            '#value' => $styleid,
  );

  $form['cssid'] = array(
            '#type' => 'value',
            '#value' => $styleset['cssid'],
  );

  $form['edit_type'] = array(
            '#type' => 'value',
            '#value' => variable_set('style_edit_editor_type','text'),
  );

  $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Delete'),
  );
 // TODO: add cancel button
  
  return $form;
}

/**
 * Implementation of hook_form_submit().
 *
 */
function style_edit_styleset_delete_form_submit($form, &$form_state) {
  $val = $form_state['values'];
  $sql = 'DELETE FROM {style_edit_cssstyles} WHERE styleid = %d';
  db_query($sql,$val['styleid']);
  set_stylesheet_modified($val['cssid']);
  drupal_set_message(t('Deleted style @sid',array('@sid' => $val['styleid'])));
  $form_state['redirect'] = redirect_to_stylescheet($val['cssid']);
}

/**
 * Implementation of hook_form().
 *
 * @param &$node The node being added or edited.
 * @param $form_state The form state array.
 * @return An array containing the form elements to be displayed in the node edit form.
 */
function style_edit_styleset_add_form($form_state, $theme, $styleid) {
  
  set_styleset_edit_breadcrumb($styleid);
  $form = set_text_editor_style_form_values(outline_text_edit_styleset(), $styleid);
  $form['sel_cat']['selector']['#default_value'] = '';
  $form['style']['css']['#default_value'] = '';
  return $form;
}

/**
 * Implementation of hook_form_submit().
 *
 */
function style_edit_styleset_add_form_submit($form, &$form_state) {
  $val = $form_state['values'];
  $sql = 'SELECT count(styleid) FROM {style_edit_cssstyles} WHERE cssid = %d';
  $result = db_query($sql,$val['cssid']);
  
  switch ($val['sel_cat']['kind']) {
    case 'STYLE':
      $update_val = array($val['sel_cat']['selector'],'{'.$val['style']['css'].'}' ,$val['styleid']);
      break;
    case 'COMMENT':
      $update_val = array($val['sel_cat']['selector'],$val['style']['css'] ,$val['styleid']);
      break;
    default:
      drupal_set_message(t('Type not known!'), 'error');
      break;
  }
  
  $newstyleset = array(
    'kind' => $val['sel_cat']['kind'],
    'selector' => $update_val[0],
    'style' => $update_val[1],
    'weight' => 1 + db_result($result),
    'cssid' => $val['cssid'],
  );
  
  drupal_write_record('style_edit_cssstyles', $newstyleset);
  
  set_stylesheet_modified($val['cssid']);
  drupal_set_message(t('Added new style set'));
  $form_state['redirect'] = redirect_to_stylescheet($val['cssid']);
}

/**
 * Implementation of hook_form().
 *
 * @param &$node The node being added or edited.
 * @param $form_state The form state array.
 * @return An array containing the form elements to be displayed in the node edit form.
 */
function style_edit_styleset_copy_form($form_state, $theme, $styleid) {
  
  $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE styleid = %d';
  $result = db_query($sql,$styleid);
  $syleset = db_fetch_array($result);
  
  $sql = 'SELECT count(styleid) FROM {style_edit_cssstyles} WHERE cssid = %d';
  $syleset['weight'] = 1 + db_result(db_query($sql,$syleset['cssid']));
  
  $newstyleset = array(
    'kind' => $syleset['kind'],
    'selector' => $syleset['selector'],
    'style' => $syleset['style'],
    'weight' => $syleset['weight'],
    'cssid' => $syleset['cssid'],
  );
  drupal_write_record('style_edit_cssstyles', $newstyleset);
  
  $styleid = db_last_insert_id('style_edit_cssstyles','styleid');
  
  set_stylesheet_modified($syleset['cssid']);
  
  drupal_set_message(t('Copied style set successful'));
  drupal_goto('admin/settings/styleedit/styleset/' . $theme . '/edit/' . $styleid);
}

?>
