<?php

/**
 * @file
 * Session based authentication plugin.
 */

/**
 * Authentication class that uses session authentication.
 */
class DeployAuthenticatorSession implements DeployAuthenticator {

  /**
   * Configuration options.
   *
   * @var array
   */
  public $config = array();

  /**
   * {@inheritdoc}
   */
  public function __construct(DeployService $service, array $config = array()) {
    $this->service = $service;
    $this->config += array(
      'debug' => FALSE,
      'username' => '',
      'password' => '',
    );
    if (!empty($config['username']) && !empty($config['password'])) {
      $config['username'] = _deploy_decrypt($config['username']);
      $config['password'] = _deploy_decrypt($config['password']);
    }
    $this->config = array_merge($this->config, $config);
  }

  /**
   * {@inheritdoc}
   */
  public function deploy(Traversable $iterator) {

    $verify_ssl = variable_get('deploy_verify_ssl', TRUE);
    $context_options = array(
      'ssl' => array(
        'verify_peer' => $verify_ssl,
        'verify_peer_name' => $verify_ssl,
        'allow_self_signed' => !$verify_ssl,
      ),
    );
    $context = stream_context_create($context_options);

    // Only authenticate if we've got a username and password. This allows for
    // deployment as an anonymous user. There are usecases for that.
    if (!empty($this->config['username'])) {
      // TODO: Consider making the resource/action path configurable. For now,
      // the default Services path is OK, since it covers 99% of the use cases.
      $login_url = $this->service->config['url'] . '/user/login';
      $logout_url = $this->service->config['url'] . '/user/logout';

      $options = array(
        'method' => 'POST',
        'headers' => array('Content-Type' => 'application/json'),
        'data' => drupal_json_encode(array(
          'username' => $this->config['username'],
          'password' => $this->config['password'],
        )),
        'context' => $context,
      );

      if ($this->config['debug']) {
        watchdog('deploy', 'Authentication request: %url <pre>@options</pre>', array('%url' => $login_url, '@options' => print_r($options, TRUE)), WATCHDOG_DEBUG);
      }

      // Login on the endpoint.
      $response = drupal_http_request($login_url, $options);

      if ($this->config['debug']) {
        watchdog('deploy', 'Authentication response: <pre>@response</pre>', array('@response' => print_r($response, TRUE)), WATCHDOG_DEBUG);
      }
      if (isset($response->error) || !in_array($response->code, array(200, 304))) {
        throw new DeployAuthenticationException(t('Authentication error: @code @error', array('@code' => $response->code, '@error' => $response->error)));
      }

      $response_data = drupal_json_decode($response->data);

      if ($response_data['session_name'] && $response_data['sessid']) {
        $this->service->config['headers']['Cookie'] = $response_data['session_name'] . "=" . $response_data['sessid'] . ";";
        // We have to get a CSFR Token from the server and pass along with
        // each services call (introduced in Services 7.x-3.4
        // as per SA-CONTRIB-2013-051; https://drupal.org/node/2012982).
        $token = isset($response_data['token']) ? $response_data['token'] : NULL;
        // To get the token directly from the login request requires Services
        // 7.x-3.6 or newer, for older versions we need a separate request.
        if (!$token) {
          $user = NULL;
          $parts = parse_url($this->service->config['url']);
          $port = '';

          if (isset($parts['port'])) {
            $port = ':' . $parts['port'];
          }

          if (isset($parts['user'])) {
            $user = $parts['user'];
          }

          if (isset($parts['pass'])) {
            $pass = $parts['pass'];
          }

          $token_url = $parts['scheme'] . '://' . ($user ? $user . ':' . $pass . '@' : '') . $parts['host'] . $port . '/services/session/token';

          $options = array(
            'method' => 'GET',
            'headers' => $this->service->config['headers'],
            'context' => $context,
          );

          $token_response = drupal_http_request($token_url, $options);
          if (isset($token_response->error) || !in_array($token_response->code, array(200, 304))) {
            throw new DeployAuthenticationException(t('Authentication error: @code @error', array('@code' => $token_response->code, '@error' => t('Failed to retrieve CSRF token.'))));
          }

          if ($this->config['debug']) {
            watchdog('deploy', 'Session CSRF Token response: @response', array('@response' => print_r($token_response, TRUE)), WATCHDOG_DEBUG);
          }

          $token = trim($token_response->data);
        }
        $this->service->config['headers']['X-CSRF-Token'] = $token;
      }
      else {
        throw new DeployAuthenticationException(t("No session was returned from the authentication request."));
      }
    }
    else {
      watchdog('deploy', 'Deployment authentication was done without user credentials.', array(), WATCHDOG_WARNING);
    }

    // Deploy the plan.
    $this->service->deploy($iterator);

    // We only need to log out if we logged in and got a session cookie.
    if (!empty($this->service->config['headers']['Cookie'])) {
      // Log out, since we are done now.
      $options = array(
        'method' => 'POST',
        'headers' => array(
          'Cookie' => $response->headers['set-cookie'],
          'X-CSRF-Token' => $token,
        ),
        'context' => $context,
      );
      $response = drupal_http_request($logout_url, $options);

      if ($this->config['debug']) {
        watchdog('deploy', 'Logout response: <pre>@response</pre>', array('@response' => print_r($response, TRUE)), WATCHDOG_DEBUG);
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function configForm(&$form_state) {
    return array(
      'username' => array(
        '#type' => 'textfield',
        '#title' => t('Username'),
        '#description' => t('Enter the username that you want to authenticate with on this endpoint.'),
        '#default_value' => $this->config['username'],
      ),
      'password' => array(
        '#type' => 'password',
        '#title' => t('Password'),
        '#description' => t('Enter the password that you want to authenticate with on this endpoint.'),
        '#default_value' => $this->config['password'],
      ),
    );
  }

}
